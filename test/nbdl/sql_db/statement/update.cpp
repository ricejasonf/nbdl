//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/sql_db/statement/update.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string_view>
#include <utility>

namespace hana = boost::hana;
using namespace std::literals;
using namespace hana::literals;

TEST_CASE("Generate an UPDATE with values for use in a prepared statement.", "[sql_db]")
{
  using nbdl::sql_db::statement::update;

  auto values = hana::make_basic_tuple(
    hana::make_pair("foo"_s, 42)
  , hana::make_pair("bar"_s, nbdl::string("I'm a bar"))
  , hana::make_pair("baz"_s, 1042)
  );

  auto keys = hana::make_basic_tuple(
    hana::make_pair("my_table_id"_s, 1)
  );

  auto result = update("my_table"_s, std::move(values), std::move(keys));

  std::string_view expected_string =
    R"RRR(UPDATE "my_table" SET "foo"=?,"bar"=?,"baz"=? WHERE "my_table_id"=? LIMIT 1;)RRR";

  CHECK(std::string_view{hana::first(result).c_str()} == expected_string);
  CHECK(hana::equal(hana::second(result), hana::make_tuple(42, nbdl::string("I'm a bar"), 1042, 1)));
}
