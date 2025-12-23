//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_sequence.hpp>
#include <nbdl/get.hpp>
#include <nbdl/ext/std/tuple_like.hpp>
#include <catch.hpp>
#include <string>
#include <tuple>

namespace hana = boost::hana;

struct some_tag { };
constexpr bool operator==(some_tag, some_tag) {
  return true;
}

TEST_CASE("Test nbdl::State for std::tuple-like types.", "[tuple_like]") {
	using Tuple = std::tuple<int, std::string, some_tag>;
  auto state = Tuple{5, "hello", {}};

  // By index
  CHECK(nbdl::get(state, nbdl::index<0>) == int{5});
  CHECK(nbdl::get(state, nbdl::index<1>) == std::string("hello"));
  CHECK(nbdl::get(state, nbdl::index<2>) == some_tag{});

  // By type
  CHECK(nbdl::get(state, nbdl::type<int>) == int{5});
  CHECK(nbdl::get(state, nbdl::type<std::string>) == std::string("hello"));
  CHECK(nbdl::get(state, nbdl::type<some_tag>) == some_tag{});
}

TEST_CASE("Test nbdl::BindableSequence for std::tuple-like types",
          "[tuple_like]") {
	using Tuple = std::tuple<int, std::string, some_tag>;
  auto state = Tuple{5, "hello", {}};

  auto result = nbdl::bind_sequence(state, [](auto const& ...xs) {
    // Note that bind_sequence filters empty objects.
    return std::tuple{xs..., some_tag{}};
  });

  CHECK(result == state);
}
