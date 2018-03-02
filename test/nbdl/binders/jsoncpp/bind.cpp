//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/AccountEntity.hpp>
#include <nbdl/binder/jsoncpp.hpp>
#include <nbdl/uid.hpp>
#include <nbdl/util/base64_decode.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <string>
#include <vector>

namespace hana = boost::hana;
using Account = account;
using nbdl::binder::jsoncpp::from_string;
using nbdl::binder::jsoncpp::to_string;

TEST_CASE("Entity can read and write to and from JSON.", "[binder][jsoncpp]") 
{
  Account account{};
  std::string input_json;

  input_json = account_entity_json_test_string;

  from_string(input_json, account);
  CHECK(account.name_first == "Jason");
  CHECK(account.name_last == "Rice");
  CHECK(account.age == 34);
  CHECK(to_string(account) == R"mmm({
   "address" : {
      "city" : "Las Vegas",
      "line1" : "  123 Spork Rd.  ",
      "line2" : "",
      "state" : "NV",
      "zip_code" : "89015"
   },
   "age" : 34,
   "email" : "ricejasonf@gmail.com",
   "food" : {
      "food_group" : {
         "name" : "Fruits"
      },
      "id" : 1,
      "name" : "Banana"
   },
   "name_first" : "Jason",
   "name_last" : "Rice",
   "phone_number" : "7024569874"
}
)mmm");
}

TEST_CASE("Container can read and write to and from JSON.", "[bind][json]") 
{
  std::string input_json;

  input_json = R"mmm([ "C++", "Haskell", "Rust", "Javascript", "PHP" ]
)mmm";

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

TEST_CASE("std::vector<unsigned char> is automatcally base64 encoded/decoded.", "[binder][jsoncpp]") 
{
  std::string input_json =  R"mmm([ "cGxlYXN1cmUu", "bGVhc3VyZS4=" ]
)mmm";

  std::vector<std::vector<unsigned char>> subject{};

  from_string(input_json, subject);
  REQUIRE(subject.size() == 2);
  CHECK(subject[0] == nbdl::util::base64_decode(std::string{"cGxlYXN1cmUu"}));
  CHECK(subject[1] == nbdl::util::base64_decode(std::string{"bGVhc3VyZS4="}));

  CHECK(to_string(subject) == input_json);
}

TEST_CASE("Bind nbdl::uid to from json", "[jsoncpp]")
{
  auto gen = nbdl::make_uid_generator();

  nbdl::uid uid = gen();

  std::string uid_json = to_string(uid);

  nbdl::uid target{};
  from_string(uid_json, target);
  CHECK(hana::equal(target, uid));
}
