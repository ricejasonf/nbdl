//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>
#include <nbdl>
#include <jsoncpp/json/json.h>

void fromString(std::string &json, Account &account)
{
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(json, root, false))
    throw std::runtime_error("JSON parse error");
  nbdl::binders::jsoncpp::Read r(root);
  nbdl::bind(r, account);
}

std::string toString(Account &account)
{
  Json::StyledWriter writer;
  Json::Value root;
  nbdl::binders::jsoncpp::Write r(root);

  nbdl::bind(r, account);
  return writer.write(root);
}

struct Bar
{
  std::string value;
};
using Foo = nbdl::Variant<std::string, int, Bar>;
struct MyEntity
{
  Foo foo;
};

namespace nbdl {
NBDL_ENTITY(Bar,
  value );
NBDL_ENTITY(MyEntity,
  foo );
}//nbdl

//note that empty types should just be represented by their type_id

std::string test_json_1 =
  "{\n"
  "   \"foo\" : 0\n"
  "}\n";
std::string test_json_2 =
  "{\n"
  "   \"foo\" : [\n"
  "     1,
  "     \"a string\"\n"
  "   ]\n"
  "}\n";
std::string test_json_3 =
  "{\n"
  "   \"foo\" : [\n"
  "     2,
  "     {\n"
  "         \"value\" : \"bar\"\n"
  "     }\n"
  "   ]\n"
  "}\n";

#include "catch.hpp"

TEST_CASE("Bind empty variant to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_1, my_entity);
  bool result = my_entity.match(
      [](nbdl::Unresolved) {
        return true;
      },
      []() {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_1);
}

TEST_CASE("Bind variant containing string to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_2, my_entity);
  bool result = my_entity.match(
      [](std::string str) {
        return str == "a string";
      },
      []() {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_2);
}

TEST_CASE("Bind variant containing entity to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_3, my_entity);
  bool result = my_entity.match(
      [](Bar bar) {
        return bar.value == "bar";
      },
      []() {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_3);
}
