//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/binder/jsoncpp.hpp>
#include <nbdl/string.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

using nbdl::binder::jsoncpp::from_string;
using nbdl::binder::jsoncpp::to_string;

std::string seq_json = "[\"Hello\",\"World!\",5,6]";
auto expected_xs = hana::make_tuple(nbdl::string("Hello"), nbdl::string("World!"), 5, 6);

TEST_CASE("Bind a sequence to JSON.", "[bind][json]") 
{
  auto xs = hana::make_tuple(nbdl::string{}, nbdl::string{}, int{}, int{});
  from_string(seq_json, xs);
  CHECK((xs == expected_xs));
  //actually prints it out
  CHECK(to_string(xs) == seq_json);
}
TEST_CASE("Bind a sequence from JSON.", "[bind][json]") 
{
  CHECK(to_string(expected_xs) == seq_json);
}

std::string seq_json_2 = "\"Moo\"";
auto expected_xs_2 = hana::make_tuple(nbdl::string("Moo"));

TEST_CASE("Bind a sequence with only element to JSON.", "[bind][json]") 
{
  auto xs = hana::make_tuple(nbdl::string{});
  from_string(seq_json_2, xs);
  CHECK((xs == expected_xs_2));
  //actually prints it out
  CHECK(to_string(xs) == seq_json_2);
}
TEST_CASE("Bind a sequence with only element from JSON.", "[bind][json]") 
{
  CHECK(to_string(expected_xs_2) == seq_json_2);
}
