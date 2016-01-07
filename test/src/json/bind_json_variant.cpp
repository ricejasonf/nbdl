//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<Bind.hpp>
#include<binders/jsoncpp/Read.hpp>
#include<binders/jsoncpp/Write.hpp>
#include<Variant.hpp>

#include<catch.hpp>
#include<jsoncpp/json/json.h>
#include<string>

template<typename T>
void fromString(std::string &json, T &my_entity)
{
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(json, root, false))
    throw std::runtime_error("JSON parse error");
  nbdl::binders::jsoncpp::Read r(root);
  nbdl::bind(r, my_entity);
}

template<typename T>
std::string toString(T &my_entity)
{
  Json::StyledWriter writer;
  Json::Value root;
  nbdl::binders::jsoncpp::Write r(root);

  nbdl::bind(r, my_entity);
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
  "   \"foo\" : [ 1, \"a string\" ]\n"
  "}\n";
std::string test_json_3 =
  "{\n"
  "   \"foo\" : [\n"
  "      3,\n"
  "      {\n"
  "         \"value\" : \"bar\"\n"
  "      }\n"
  "   ]\n"
  "}\n";

TEST_CASE("Bind empty variant to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_1, my_entity);
  bool result = my_entity.foo.match(
      [](nbdl::Unresolved) {
        return true;
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_1);
}

TEST_CASE("Bind variant containing string to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_2, my_entity);
  bool result = my_entity.foo.match(
      [](std::string str) {
        return str == "a string";
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_2);
}

TEST_CASE("Bind variant containing entity to JSON.", "[bind][json]") 
{
  MyEntity my_entity;
  fromString(test_json_3, my_entity);
  bool result = my_entity.foo.match(
      [](Bar bar) {
        return bar.value == "bar";
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(toString(my_entity) == test_json_3);
}
