//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_STATEMENT_SELECT_HPP
#define NBDL_SQL_DB_STATEMENT_SELECT_HPP

#include <mpdef/list.hpp>
#include <nbdl/sql_db/detail/string_sum.hpp>

static_assert(
  BOOST_HANA_CONFIG_ENABLE_STRING_UDL
, "nbdl::sql_db requires BOOST_HANA_CONFIG_ENABLE_STRING_UDL to be set... sorry."
);

#include <boost/hana/first.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/string.hpp>
#include <utility>

namespace nbdl::sql_db::statement
{
  namespace hana = boost::hana;
  using namespace hana::literals;

  namespace select_detail
  {
    template <typename Pair1, typename ...PairN>
    constexpr auto criteria_list(hana::basic_tuple<Pair1, PairN...> const&)
    {
      return decltype(detail::string_sum(
        std::declval<Pair1>()
      , detail::string_sum("=?,"_s, std::declval<PairN>())...
      , "=?"_s
      )){};
    };
  }

  constexpr auto select = [](auto table_name
                           , auto columns_clause
                           , auto join_clauses
                           , auto&& keys)
  {
    auto stmt = detail::string_sum(
      "SELECT "_s
    , columns_clause
    , " FROM "_s, detail::identifier_quote<decltype(table_name)>{}
    , join_clauses
    , " WHERE "_s, select_detail::criteria_list(hana::keys(keys))
    , ";"_s
    );

    return hana::make_pair(
      stmt
    , hana::values(std::forward<decltype(keys)>(keys))
    );
  };
}

#endif
