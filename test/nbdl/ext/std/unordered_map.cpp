//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/ext/std/unordered_map.hpp>
#include <nbdl/match.hpp>

#include <catch.hpp>
#include <string>
#include <utility>

namespace hana = boost::hana;

TEST_CASE("Match on values in an std::unordered_map.", "[ext]")
{
  std::unordered_map<int, std::string> map{};
  map[42] = std::string("hello");
  map[101] = std::string("world");

  std::size_t initial_size = map.size();

  {
    bool result = false;
    nbdl::match(map, 42, hana::overload_linearly(
      [](nbdl::uninitialized) { }
    , [&](std::string& x)
      {
        result = x == map[42];
      }
    ));
    CHECK(result);
  }
  {
    bool result = false;
    nbdl::match(map, 101, hana::overload_linearly(
      [](nbdl::uninitialized) { }
    , [&](std::string& x)
      {
        result = x == map[101];
        x += '!';
      }
    ));
    CHECK(map[101] == std::string("world!"));
    CHECK(result);
  }
  {
    bool result = false;
    nbdl::match(map, 1234, hana::overload_linearly(
      [&](nbdl::uninitialized) { result = true; }
    , [](std::string&) { }
    ));
    CHECK(result);
  }

  CHECK(map.size() == initial_size);
}
