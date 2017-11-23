//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_STATEMENT_UPDATE_HPP
#define NBDL_SQL_DB_STATEMENT_UPDATE_HPP

#include <mpdef/list.hpp>
#include <nbdl/sql_db/detail/string_sum.hpp>
#include <nbdl/string.hpp>

#include <utility>

static_assert(
  BOOST_HANA_CONFIG_ENABLE_STRING_UDL
, "nbdl::sql_db requires BOOST_HANA_CONFIG_ENABLE_STRING_UDL to be set... sorry."
);

#include <boost/hana/pair.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/sum.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/mp11/algorithm.hpp>
#include <utility>

namespace nbdl::sql_db::statement
{
  namespace hana = boost::hana;
  using namespace boost::mp11;
  using namespace hana::literals;

  namespace update_detail
  {
    template <typename Pair1, typename ...PairN>
    constexpr auto column_value_list(hana::basic_tuple<Pair1, PairN...> const&)
    {
      return decltype(detail::string_sum(
        detail::string_sum("\""_s, hana::first(std::declval<Pair1>()))
      , detail::string_sum("\"=?,\""_s, hana::first(std::declval<PairN>()))...
      , "\"=?"_s
      )){};
    };
  }

  constexpr auto update = [](auto table_name, auto&& columns_, auto&& keys_)
  {
    // These must be hana::basic_tuple
    // That's not being checked here
    auto columns  = std::forward<decltype(columns_)>(columns_);
    auto keys     = std::forward<decltype(keys_)>(keys_);

    auto stmt = detail::string_sum(
      "UPDATE \""_s, table_name
    , "\" SET "_s
    , update_detail::column_value_list(columns)
    , " WHERE "_s, update_detail::column_value_list(keys)
    , " LIMIT 1;"_s
    );

    return hana::make_pair(
      stmt
    , hana::concat(
        hana::transform(std::move(columns), hana::second)
      , hana::transform(std::move(keys), hana::second)
      )
    );
  };
}

#endif
