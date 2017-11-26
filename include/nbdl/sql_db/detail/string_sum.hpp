//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_DETAIL_STRING_SUM_HPP
#define NBDL_SQL_DB_DETAIL_STRING_SUM_HPP

#include <mpdef/list.hpp>

#include <boost/hana/plus.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/sum.hpp>

namespace nbdl::sql_db::detail
{
  namespace hana = boost::hana;

  constexpr auto string_sum = [](auto ...xs)
  {
    return hana::sum<hana::string_tag>(mpdef::make_list(xs...));
  };

  template <typename Delimeter = hana::string<' '>>
  constexpr auto string_sum_join = [](auto x1, auto ...xs)
  {
    return string_sum(x1, hana::plus(Delimeter{}, xs)...);
  };

  using identifier_quote_string = hana::string<'"'>; // SQL implementation specific (ANSI)

  template <typename Identifier>
  using identifier_quote = decltype(
    hana::plus(identifier_quote_string{},
    hana::plus(Identifier{},
      identifier_quote_string{}
    ))
  );
}

#endif
