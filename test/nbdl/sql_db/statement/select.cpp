//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/sql_db/statement/select.hpp>
#include <nbdl/string.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string_view>
#include <type_traits>

namespace hana = boost::hana;
using namespace std::literals;
using namespace hana::literals;

TEST_CASE("Generate SELECT prepared statement string with values.", "[sql_db]")
{
  using nbdl::sql_db::statement::select;

  auto result = select(
    "my_table"_s
  , nbdl::sql_db::detail::string_sum_join<hana::string<','>>(
      "\"foo\".\"bar\""_s
    , "\"foo\".\"baz\""_s
    , "\"moo\".\"bar\""_s
    , "\"moo\".\"baz\""_s
    )
  , " JOINS"_s // joins
  , hana::make_map(hana::make_pair("\"my_table\".\"my_table_id\""_s, 5))
  );

  std::string_view expected_string =
    R"RRR(SELECT "foo"."bar","foo"."baz","moo"."bar","moo"."baz" FROM "my_table" JOINS WHERE "my_table"."my_table_id"=?;)RRR";

  CHECK(std::string_view{hana::first(result).c_str()} == expected_string);
  CHECK(hana::equal(hana::second(result), hana::make_basic_tuple(5)));
}
