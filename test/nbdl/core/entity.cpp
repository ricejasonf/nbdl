//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/entity.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

struct person
{
  std::string name;
  int age;
};

namespace nbdl
{
  NBDL_ENTITY(
    person
    , name
    , age
  );
} // nbdl

TEST_CASE("Entity equality.", "[entity]") 
{
  person p1{"Skippy", 42};
  person p2{"Skippy", 42};
  person p3{"Skippy", 43};
  person p4{"Skippyz", 42};

  CHECK(hana::equal(p1, p2));
  CHECK(!hana::equal(p1, p3));
  CHECK(!hana::equal(p1, p4));
}
