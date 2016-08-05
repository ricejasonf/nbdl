//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_STRING_HPP
#define NBDL_MPDEF_STRING_HPP

#include <mpdef/list.hpp>

#include <boost/hana/intersperse.hpp>
#include <boost/hana/string.hpp>

namespace mpdef
{

  namespace hana = boost::hana;

  struct string_concat_fn
  {
    template <char ...c1, char ...c2>
    constexpr auto operator()(hana::string<c1...>, hana::string<c2...>) const
    {
      return hana::string<c1..., c2...>{};
    }
  };

  constexpr string_concat_fn string_concat{};

  struct string_join_fn
  {
    template <typename Delimiter>
    constexpr auto operator()(Delimiter d) const
    {
      return hana::partial(string_join_fn{}, d);
    }

    template <typename Delimiter, typename String1, typename ...StringN>
    constexpr auto operator()(Delimiter d, String1 s1, StringN ...sN) const
    {
      return hana::fold_left(
        hana::intersperse(mpdef::make_list(s1, sN...), d),
        string_concat
      );
    }
  };

  constexpr string_join_fn string_join{};
}
#endif
