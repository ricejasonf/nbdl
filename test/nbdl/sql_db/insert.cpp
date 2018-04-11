//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_sequence.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/sql_db/insert.hpp>
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

  struct baz
  {
    int baz_value;
    foo baz_foo;
    bar baz_bar;
  };

  struct boo
  {
    std::vector<foo> foo_list;
  };

  template <typename T>
  std::string value_to_string(nbdl::key_impl<T> const& v)
  {
    return std::to_string(v.value);
  };

  std::string value_to_string(std::string const& v)
  {
    return v;
  };

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

  uint32_t key__;
  statement_list_t statements;

  auto init_test_data = []
  {
    key__ = 0;
    statements = statement_list_t{};
  };

  auto db_insert = [](auto table_name, auto&& params)
  {
    statements.push_back(
      std::string("INSERT<")
    + std::string(table_name.c_str(), hana::length(table_name))
    + '>'
    + map_to_string(std::forward<decltype(params)>(params))
    );

    return ++key__;
  };
}

namespace nbdl
{
  NBDL_ENTITY(foo, foo_value);
  NBDL_ENTITY(bar, bar_value, bar_foo);
  NBDL_ENTITY(baz, baz_value, baz_foo, baz_bar);
  NBDL_ENTITY(boo, foo_list);
}

TEST_CASE("Insert a flat entity.", "[sql_db]")
{
  init_test_data();
  using nbdl::sql_db::insert;
  using nbdl::sql_db::primary_key;

  primary_key<foo> result_id = insert(db_insert, foo{42});

  statement_list_t expected_statements{
    std::string("INSERT<foo> foo_value:42")
  };

  CHECK(result_id.value == 1);
  CHECK(statements == expected_statements);
}

TEST_CASE("Insert entity with nested entities.", "[sql_db]")
{
  init_test_data();
  using nbdl::sql_db::insert;
  using nbdl::sql_db::primary_key;

  auto input = baz{
    int{7}
  , foo{int{42}}
  , bar{
      nbdl::string("moo")
    , foo{int{82}}
    }
  };

  primary_key<baz> result_id = insert(db_insert, input);

  statement_list_t expected_statements{
    std::string("INSERT<foo> foo_value:42")
  , std::string("INSERT<foo> foo_value:82")
  , std::string("INSERT<bar> bar_foo_id:2 bar_value:moo")
  , std::string("INSERT<baz> baz_foo_id:1 baz_bar_id:3 baz_value:7")
  };

  CHECK(result_id.value == 4);
  CHECK(statements == expected_statements);
}

TEST_CASE("Insert a Container of entities.", "[sql_db]")
{
  init_test_data();
  using nbdl::sql_db::insert;
  using nbdl::sql_db::primary_key;

  auto input = boo{
    std::vector<foo>{foo{101}, foo{221}, foo{123}}
  };

  primary_key<boo> result_id = insert(db_insert, input);

  statement_list_t expected_statements{
    std::string("INSERT<boo>")
  , std::string("INSERT<foo> boo_id:1 foo_value:101")
  , std::string("INSERT<foo> boo_id:1 foo_value:221")
  , std::string("INSERT<foo> boo_id:1 foo_value:123")
  };

  CHECK(result_id.value == 1);
  CHECK(statements == expected_statements);
}
