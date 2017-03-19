//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/path.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

#define CHECK_TYPE_EQUAL(RESULT, EXPECTED) BOOST_HANA_CONSTANT_CHECK(hana::equal( \
  hana::typeid_(RESULT), hana::typeid_(EXPECTED) \
));

namespace
{
  template <typename>
  struct key;
};

TEST_CASE("Make a root path", "[path]")
{
  constexpr auto result = nbdl::root_path(hana::int_c<0>);
  constexpr auto expected = hana::type_c<nbdl::path_t<hana::int_<0>>>;

  CHECK_TYPE_EQUAL(result, expected);
}

TEST_CASE("Append to a path", "[path]")
{
  constexpr auto result = nbdl::path(
    nbdl::root_path(hana::int_c<0>)
  , hana::int_c<1>
  );

  constexpr auto expected = hana::type_c<nbdl::path_t<hana::int_<0>, hana::int_<1>>>;

  CHECK_TYPE_EQUAL(result, expected);
}

TEST_CASE("Make path node with run-time state depend on another path", "[path]")
{
  constexpr auto result = nbdl::path(
    nbdl::root_path(hana::int_c<0>)
  , nbdl::path_node<key<int>>(nbdl::root_path(hana::int_c<42>))
  );

  constexpr auto expected = hana::type_c<nbdl::path_t<
    hana::int_<0>
  , nbdl::path_node_t<key<int>, nbdl::path_t<hana::int_<42>>>
  >>;

  CHECK_TYPE_EQUAL(result, expected);
}
