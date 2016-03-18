//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<Bind.hpp>
#include<binders/jsoncpp/Read.hpp>
#include<binders/jsoncpp/Write.hpp>
#include<assets/AccountEntity.hpp>

#include<boost/hana.hpp>
#include<catch.hpp>
#include<jsoncpp/json/json.h>
#include<string>

using Account = account;

void from_string(std::string &json, Account &account)
{
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(json, root, false))
    throw std::runtime_error("JSON parse error");
  nbdl::binders::jsoncpp::read r(root);
  nbdl::bind(r, account);
}

std::string to_string(Account &account)
{
  Json::StyledWriter writer;
  Json::Value root;
  nbdl::binders::jsoncpp::write r(root);

  nbdl::bind(r, account);
  return writer.write(root);
}

TEST_CASE("Entity can read and write to and from JSON.", "[bind][json]") 
{
  Account account{};
  std::string input_json, output_json;

  input_json = account_entity_json_test_string;

  from_string(input_json, account);
    REQUIRE(account.name_first == "Jason");
    REQUIRE(account.name_last == "Rice");
    REQUIRE(account.age == 34);
  REQUIRE(to_string(account) == "{\n   \"address\" : {\n      \"city\" : \"Las Vegas\",\n      \"line1\" : \"  123 Spork Rd.  \",\n      \"line2\" : \"\",\n      \"state\" : \"NV\",\n      \"zip_code\" : \"89015\"\n   },\n   \"age\" : 34,\n   \"email\" : \"ricejasonf@gmail.com\",\n   \"food\" : {\n      \"food_group\" : {\n         \"name\" : \"Fruits\"\n      },\n      \"id\" : 1,\n      \"name\" : \"Banana\"\n   },\n   \"name_first\" : \"Jason\",\n   \"name_last\" : \"Rice\",\n   \"phone_number\" : \"7024569874\"\n}\n");
}
