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
void from_string(std::string &json, T &my_entity)
{
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(json, root, false))
    throw std::runtime_error("JSON parse error");
  nbdl::binders::jsoncpp::read r(root);
  nbdl::bind(r, my_entity);
}

template<typename T>
std::string to_string(T &my_entity)
{
  Json::StyledWriter writer;
  Json::Value root;
  nbdl::binders::jsoncpp::write r(root);

  nbdl::bind(r, my_entity);
  return writer.write(root);
}

struct bar
{
  std::string value;
};
using Foo = nbdl::variant<std::string, int, bar>;
struct my_entity
{
  Foo foo;
};

namespace nbdl {
NBDL_ENTITY(bar,
  value );
NBDL_ENTITY(my_entity,
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
  my_entity my_entity;
  from_string(test_json_1, my_entity);
  bool result = my_entity.foo.match(
      [](nbdl::unresolved) {
        return true;
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_1);
}

TEST_CASE("Bind variant containing string to JSON.", "[bind][json]") 
{
  my_entity my_entity;
  from_string(test_json_2, my_entity);
  bool result = my_entity.foo.match(
      [](std::string str) {
        return str == "a string";
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_2);
}

TEST_CASE("Bind variant containing entity to JSON.", "[bind][json]") 
{
  my_entity my_entity;
  from_string(test_json_3, my_entity);
  bool result = my_entity.foo.match(
      [](bar bar) {
        return bar.value == "bar";
      },
      [](auto) {
        return false;
      });
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_3);
}
