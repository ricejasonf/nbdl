//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/string_concat_view.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

#include <iostream>

namespace hana = boost::hana;

namespace
{
  constexpr hana::string<'f', 'o', 'o'> foo_s;
  constexpr hana::string<'b', 'a', 'r'> bar_s;
  constexpr hana::string<'b', 'a', 'z'> baz_s;
}

TEST_CASE("Default/Copy construct string_concat_view.", "[detail][string_concat_view]")
{
  nbdl::detail::string_concat_view result1{};
  nbdl::detail::string_concat_view result = result1;

  std::string expected("");

  // TODO use string_concat_view::equal
  CHECK(result.to_string() == expected);
}

TEST_CASE("Concat strings from a tuple of string like objects.", "[detail][string_concat_view]")
{
  auto strings = hana::make_tuple(
    std::string("Hello")
  , hana::string<',', ' '>{}
  , std::string("World")
  , std::string("!")
  , std::string()
  , foo_s
  , bar_s
  , baz_s
  );

  nbdl::detail::string_concat_view result = hana::unpack(
    strings
  , nbdl::detail::make_string_concat_view
  );

  std::string expected("Hello, World!foobarbaz");

  CHECK(result.to_string() == expected);
}
