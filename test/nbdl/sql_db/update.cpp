//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_sequence.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/sql_db/update.hpp>
#include <nbdl/string.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace hana = boost::hana;
using namespace std::literals;

namespace
{
  struct foo
  {
    int foo_value;
  };

  struct bar
  {
    nbdl::string bar_value;
    foo bar_foo;
  };

  template <typename T>
  std::string value_to_string(nbdl::key_impl<T> const& v)
  {
    return std::to_string(v.value);
  };

#if 0 // unused
  std::string value_to_string(std::string const& v)
  {
    return v;
  };
#endif

  std::string value_to_string(uint32_t const& v)
  {
    return std::to_string(v);
  };

  template <typename T>
  std::string column_to_string(T const& pair)
  {
    return std::string(" ")
         + std::string(hana::first(pair).c_str(), hana::size(hana::first(pair)))
         + ':'
         + value_to_string(hana::second(pair))
         ;
  };

  constexpr auto map_to_string = [](auto&& map)
    -> std::string
  {
    return nbdl::bind_sequence(std::forward<decltype(map)>(map), [](auto const& ...pairs)
    {
      return (column_to_string(std::forward<decltype(pairs)>(pairs)) + ... + "");
    });
  };

  using statement_list_t = std::vector<std::string>;

  statement_list_t statements;

  auto init_test_data = []
  {
    statements = statement_list_t{};
  };

  constexpr auto db_update = [](auto table_name, auto&& values, auto&& keys)
  {
    statements.push_back(
      std::string("UPDATE<")
    + std::string(table_name.c_str(), hana::length(table_name))
    + '>'
    + map_to_string(std::forward<decltype(values)>(values))
    + " WHERE"
    + map_to_string(std::forward<decltype(keys)>(keys))
    );
  };
}

namespace nbdl
{
  NBDL_ENTITY(foo, foo_value);
  NBDL_ENTITY(bar, bar_value, bar_foo);
}

TEST_CASE("Update a flat entity.", "[sql_db]")
{
  init_test_data();
  using nbdl::sql_db::update;
  using nbdl::sql_db::primary_key;
  using nbdl::sql_db::table_key_name;

  update(
    db_update
  , foo{42}
  , hana::make_tuple(
      hana::make_pair(table_key_name<foo>{}, primary_key<foo>{1})
    )
  );

  statement_list_t expected_statements{
    std::string("UPDATE<foo> foo_value:42 WHERE foo_id:1")
  };

  CHECK(statements == expected_statements);
}
