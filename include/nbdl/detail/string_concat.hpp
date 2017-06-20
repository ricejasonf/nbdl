//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_STRING_CONCAT_HPP
#define NBDL_DETAIL_STRING_CONCAT_HPP

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
#include <experimental/string_view>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  struct to_string_view_fn
  {
    using string_view = std::experimental::string_view;

    template <typename String>
    string_view operator()(String const& str) const
    {
      if constexpr(hana::is_a<std::string, decltype(std::cref(str).get())>)
      {
        return string_view(std::cref(str).get());
      }
      else if constexpr(hana::is_a<hana::string_tag, String>)
      {
        return string_view(hana::to<char const*>(str), decltype(hana::length(str))::value);
      }
      else
      {
        return string_view(hana::to<char const*>(std::cref(str).get()));
      }
    }
  };

  constexpr to_string_view_fn to_string_view{};

  struct string_concat_fn
  {
    template <typename Strings>
    auto operator()(Strings const& strings) const
    {
      using string_view = std::experimental::string_view;
      static_assert(hana::Foldable<Strings>::value, "Strings must be a hana::Foldable");

      auto views = hana::unpack(strings, [](auto const& ...x)
      {
        return std::array<string_view, sizeof...(x)>{{to_string_view(x)...}};
      });

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
  };

  constexpr string_concat_fn string_concat{};
}

#endif
