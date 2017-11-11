//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/sql_db/statement/insert.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string_view>
#include <type_traits>

namespace hana = boost::hana;
using namespace std::literals;
using namespace hana::literals;

TEST_CASE("Generate an INSERT with values for use in a prepared statement.", "[sql_db]")
{
  using nbdl::sql_db::statement::insert;

  auto result = insert("my_table"_s, hana::make_map(
    hana::make_pair("foo"_s, 42)
  , hana::make_pair("bar"_s, nbdl::string("I'm a bar"))
  , hana::make_pair("baz"_s, 1042)
  ));

  std::string_view expected_string =
    R"RRR(INSERT INTO "my_table" ("foo","bar","baz") VALUES (?,?,?);)RRR";

  CHECK(std::string_view{hana::first(result).c_str()} == expected_string);
  CHECK(hana::equal(hana::second(result), hana::make_tuple(42, nbdl::string("I'm a bar"), 1042)));
}
