//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>
#include <boost/hana.hpp>
#include <nbdl>
#include <jsoncpp/json/json.h>
#include "../catch.hpp"

namespace hana = boost::hana;

template<typename T>
void fromString(std::string &json, T& t)
{
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(json, root, false))
    throw std::runtime_error("JSON parse error");
  nbdl::binders::jsoncpp::Read r(root);
  nbdl::bind(r, t);
}

template<typename T>
std::string toString(T&& t)
{
  Json::StyledWriter writer;
  Json::Value root;
  nbdl::binders::jsoncpp::Write r(root);

  nbdl::bind(r, std::forward<T>(t));
  return writer.write(root);
}

std::string seq_json = "[ \"Hello\", \"World!\", 5, 6 ]\n";
auto expected_xs = hana::make_tuple(std::string("Hello"), std::string("World!"), 5, 6);

TEST_CASE("Bind a sequence to JSON.", "[bind][json]") 
{
  auto xs = hana::make_tuple(std::string{}, std::string{}, int{}, int{});
  fromString(seq_json, xs);
  CHECK((xs == expected_xs));
  //actually prints it out
  CHECK(toString(xs) == seq_json);
}
TEST_CASE("Bind a sequence from JSON.", "[bind][json]") 
{
  CHECK(toString(expected_xs) == seq_json);
}

std::string seq_json_2 = "\"Moo\"\n";
auto expected_xs_2 = hana::make_tuple(std::string("Moo"));

TEST_CASE("Bind a sequence with only element to JSON.", "[bind][json]") 
{
  auto xs = hana::make_tuple(std::string{});
  fromString(seq_json_2, xs);
  CHECK((xs == expected_xs_2));
  //actually prints it out
  CHECK(toString(xs) == seq_json_2);
}
TEST_CASE("Bind a sequence with only element from JSON.", "[bind][json]") 
{
  CHECK(toString(expected_xs_2) == seq_json_2);
}
