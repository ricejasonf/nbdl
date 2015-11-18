//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<boost/hana.hpp>
#include<FindByKey.hpp>
#include "catch.hpp"

namespace hana = boost::hana;

TEST_CASE("Create a constexpr tuple of pairs and findByFirst.", "[hana]") 
{
  constexpr auto tup = hana::make_tuple(
    hana::make_pair(hana::int_c<0>, hana::int_c<10>),
    hana::make_pair(hana::int_c<1>, hana::int_c<11>),
    hana::make_pair(hana::int_c<2>, hana::int_c<12>),
    hana::make_pair(hana::int_c<3>, hana::int_c<13>));

  CHECK(nbdl::findByKey(tup, hana::int_c<0>) == hana::int_c<10>);
  CHECK(nbdl::findByKey(tup, hana::int_c<1>) == hana::int_c<11>);
  CHECK(nbdl::findByKey(tup, hana::int_c<2>) == hana::int_c<12>);
  CHECK(nbdl::findByKey(tup, hana::int_c<3>) == hana::int_c<13>);
}

TEST_CASE("Create a run-time tuple of pairs and findByFirst to get a reference.", "[hana]") 
{
  auto tup = hana::make_tuple(
    hana::make_pair(BOOST_HANA_STRING("one"),   int()),
    hana::make_pair(BOOST_HANA_STRING("two"),   int()),
    hana::make_pair(BOOST_HANA_STRING("three"), int()),
    hana::make_pair(BOOST_HANA_STRING("four"),  int()));

  nbdl::findByKey(tup, BOOST_HANA_STRING("one"))   = 1;
  nbdl::findByKey(tup, BOOST_HANA_STRING("two"))   = 2;
  nbdl::findByKey(tup, BOOST_HANA_STRING("three")) = 3;
  nbdl::findByKey(tup, BOOST_HANA_STRING("four"))  = 4;

  CHECK(nbdl::findByKey(tup, BOOST_HANA_STRING("one"))   == 1);
  CHECK(nbdl::findByKey(tup, BOOST_HANA_STRING("two"))   == 2);
  CHECK(nbdl::findByKey(tup, BOOST_HANA_STRING("three")) == 3);
  CHECK(nbdl::findByKey(tup, BOOST_HANA_STRING("four"))  == 4);
}
