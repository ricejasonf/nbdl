//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/binder/jsoncpp.hpp>
#include <assets/AccountEntity.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string>
#include <vector>

using Account = account;

using nbdl::binder::jsoncpp::from_string;
using nbdl::binder::jsoncpp::to_string;

TEST_CASE("Entity can read and write to and from JSON.", "[bind][json]") 
{
  Account account{};
  std::string input_json, output_json;

  input_json = account_entity_json_test_string;

  from_string(input_json, account);
  CHECK(account.name_first == "Jason");
  CHECK(account.name_last == "Rice");
  CHECK(account.age == 34);
  CHECK(to_string(account) == "{\n   \"address\" : {\n      \"city\" : \"Las Vegas\",\n      \"line1\" : \"  123 Spork Rd.  \",\n      \"line2\" : \"\",\n      \"state\" : \"NV\",\n      \"zip_code\" : \"89015\"\n   },\n   \"age\" : 34,\n   \"email\" : \"ricejasonf@gmail.com\",\n   \"food\" : {\n      \"food_group\" : {\n         \"name\" : \"Fruits\"\n      },\n      \"id\" : 1,\n      \"name\" : \"Banana\"\n   },\n   \"name_first\" : \"Jason\",\n   \"name_last\" : \"Rice\",\n   \"phone_number\" : \"7024569874\"\n}\n");
}

TEST_CASE("Container can read and write to and from JSON.", "[bind][json]") 
{
  std::string input_json, output_json;

  input_json = "[ \"C++\", \"Haskell\", \"Rust\", \"Javascript\", \"PHP\" ]\n";

  std::vector<nbdl::string> languages;

  from_string(input_json, languages);
  REQUIRE(languages.size() == 5);
  CHECK(languages[0] == "C++");
  CHECK(languages[1] == "Haskell");
  CHECK(languages[2] == "Rust");
  CHECK(languages[3] == "Javascript");
  CHECK(languages[4] == "PHP");

  CHECK(to_string(languages) == input_json);
}
