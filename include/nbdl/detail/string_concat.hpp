//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_STRING_CONCAT_HPP
#define NBDL_DETAIL_STRING_CONCAT_HPP

#include <algorithm>
#include <array>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/core/to.hpp>
#include <boost/hana/ext/std/array.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/sum.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/value.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;
  using string_view = std::string_view;

  struct to_string_view_fn
  {
    template <typename String>
    string_view operator()(String const& str) const
    {
      if constexpr(hana::is_a<std::string, String>)
      {
        return string_view(str);
      }
      else if constexpr(hana::is_a<hana::string_tag, String>)
      {
        return string_view(hana::to<char const*>(str), decltype(hana::length(str))::value);
      }
      else
      {
        return string_view(hana::to<char const*>(str));
      }
    }
  };

  constexpr to_string_view_fn to_string_view{};

  struct string_concat_fn
  {
    template <typename Views>
    auto with_views(Views const& views) const
    {
      std::size_t length = hana::unpack(views, [](auto const& ...x)
      {
        return (x.length() + ... + 0);
      });

      std::string result(length, 0);
      auto current = result.begin();

      hana::for_each(views, [&](auto const& view)
      {
        std::copy(view.begin(), view.end(), current);
        current += view.length();
      });

      return result;
    }

    template <typename Strings>
    auto operator()(Strings const& strings) const
    {
      return with_views(hana::unpack(strings, [](auto const& ...x)
      {
        return std::array<string_view, sizeof...(x)>{{to_string_view(x)...}};
      }));
    }
  };

  constexpr string_concat_fn string_concat{};
}

#endif
