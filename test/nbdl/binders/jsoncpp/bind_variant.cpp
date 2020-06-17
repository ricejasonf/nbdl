//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/binder/jsoncpp.hpp>
#include <nbdl/string.hpp>
#include <nbdl/variant.hpp>

#include <catch.hpp>
#include <string>

using nbdl::binder::jsoncpp::from_string;
using nbdl::binder::jsoncpp::to_string;

struct bar
{
  nbdl::string value;
};
using Foo = nbdl::variant<nbdl::string, int, bar>;
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

// note that empty types should just be represented by their type_id

std::string test_json_1 =
  "{\"foo\":0}";
std::string test_json_2 =
  "{\"foo\":[1,\"a string\"]}";
std::string test_json_3 =
  "{\"foo\":[3,{\"value\":\"bar\"}]}";

TEST_CASE("Bind empty variant to JSON.", "[bind][json]") 
{
  my_entity my_entity;
  from_string(test_json_1, my_entity);
  bool result = false;
  my_entity.foo.match(
    [&](nbdl::unresolved) { result = true; }
  , [](auto) { }
  );
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_1);
}

TEST_CASE("Bind variant containing string to JSON.", "[bind][json]") 
{
  my_entity my_entity;
  from_string(test_json_2, my_entity);
  bool result = false;
  my_entity.foo.match(
    [&](nbdl::string str) { result = str == "a string"; }
  , [](auto) { }
  );
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_2);
}

TEST_CASE("Bind variant containing entity to JSON.", "[bind][json]") 
{
  my_entity my_entity;
  from_string(test_json_3, my_entity);
  bool result = false;
  my_entity.foo.match(
    [&](bar bar) { result = bar.value == "bar"; }
  , [](auto) { }
  );
  CHECK(result);
  CHECK(to_string(my_entity) == test_json_3);
}
