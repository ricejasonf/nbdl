//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_sequence.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/sql_db/select.hpp>
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

  std::string value_to_string(uint32_t const& v)
  {
    return std::to_string(v);
  };
#endif

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

  template <typename Result>
  constexpr auto db_select = [](auto table_name
                              , auto columns_clause
                              , auto join_clauses
                              , auto&& keys)
  {
    statements.push_back(
      std::string("SELECT<")
    + std::string(table_name.c_str(), hana::length(table_name))
    + '>'
    + " COLUMNS "
    + std::string(columns_clause.c_str(), hana::length(columns_clause))
    + " JOINS"
    + std::string(join_clauses.c_str(), hana::length(join_clauses))
    + " WHERE"
    + map_to_string(std::forward<decltype(keys)>(keys))
    );

    // Just default construct for the test I guess.
    // It is really a detail of the concrete database
    // connector implementation.
    return Result{};
  };
}

namespace nbdl
{
  NBDL_ENTITY(foo, foo_value);
  NBDL_ENTITY(bar, bar_value, bar_foo);
}

TEST_CASE("SELECT an entity entity.", "[sql_db]")
{
  init_test_data();
  using nbdl::sql_db::select;
  using nbdl::sql_db::primary_key;
  using nbdl::sql_db::select_primary_key_name;

  auto result = select<bar>(
    db_select<bar>
  , hana::make_basic_tuple(
      hana::make_pair(select_primary_key_name<bar>{}, primary_key<bar>{101})
    )
  );

  statement_list_t expected_statements{
    std::string(R"MOO(SELECT<bar> COLUMNS "bar"."bar_value","bar.bar_foo"."foo_value" JOINS JOIN "foo" AS "bar.bar_foo" ON "bar"."bar_foo_id" = "bar.bar_foo"."foo_id" WHERE "bar"."bar_id":101)MOO")
  };

  CHECK(statements == expected_statements);
  CHECK(hana::equal(result, bar{}));
}
