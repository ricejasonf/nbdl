//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_STATEMENT_INSERT_HPP
#define NBDL_SQL_DB_STATEMENT_INSERT_HPP

#include <mpdef/list.hpp>
#include <nbdl/sql_db/detail/string_sum.hpp>
#include <nbdl/string.hpp>

static_assert(
  BOOST_HANA_CONFIG_ENABLE_STRING_UDL
, "nbdl::sql_db requires BOOST_HANA_CONFIG_ENABLE_STRING_UDL to be set... sorry."
);

#include <boost/hana/basic_tuple.hpp>
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

  namespace insert_detail
  {
    template <typename Name1, typename ...NameN>
    constexpr auto column_name_list(hana::basic_tuple<Name1, NameN...>)
    {
      return detail::string_sum(
        detail::string_sum("\""_s, Name1{})
      , detail::string_sum("\",\""_s, NameN{})...
      , "\""_s
      );
    };

    template <std::size_t count>
    constexpr auto placeholder_list(hana::size_t<count>)
    {
      return hana::sum<hana::string_tag>(
        mp_append<
          mpdef::list<hana::string<'?'>>
        , mp_repeat_c<mpdef::list<hana::string<',', '?'>>, count - 1>
        >{}
      );
    };
  }

  constexpr auto insert = [](auto table_name, auto&& columns)
  {
    // columns is a hana::map with pair<name, column>
    auto stmt = detail::string_sum(
      "INSERT INTO \""_s, table_name
    , "\" ("_s
    , insert_detail::column_name_list(hana::keys(columns))
    , ") VALUES ("_s
    , insert_detail::placeholder_list(hana::size(columns))
    , ");"_s
    );

    return hana::make_pair(
      stmt
    , hana::values(std::forward<decltype(columns)>(columns))
    );
  };
}

#endif
