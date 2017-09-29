//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/entity.hpp>
#include <nbdl/delta.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

namespace delta_test_entity
{
  struct person
  {
    std::string name_first;
    std::string name_last;
    int age;
  };
}

namespace nbdl
{
  NBDL_ENTITY(
    delta_test_entity::person
    , name_first
    , name_last
    , age
  );
}

TEST_CASE("Make delta from one object", "[delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{};

  auto d1 = nbdl::make_delta<nbdl::delta_tag>(p1);
  nbdl::apply_delta(d1, p2);

  CHECK(hana::equal(p1, p2));
}

TEST_CASE("apply_delta mutate object in place", "[delta][apply_delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{"Skippy", "McGeez", 43};
  person p3{"Scrappy", "Doo", 6};

  auto d = nbdl::make_delta<nbdl::delta_tag>(p2, p1);

  CHECK(nbdl::apply_delta(d, p3));

  CHECK("Scrappy" == p3.name_first);
  CHECK("McGeez"  == p3.name_last);
  CHECK(43        == p3.age);

  // check idempotence
  CHECK(!nbdl::apply_delta(d, p3));

  CHECK("Scrappy" == p3.name_first);
  CHECK("McGeez"  == p3.name_last);
  CHECK(43        == p3.age);
}

TEST_CASE("apply_delta mutate delta in place", "[delta][apply_delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{"Skippy", "McGeez", 43};
  person p3{"Scrappy", "Doo", 6};
  person p4{"Scrappy", "Doo", 500};

  auto d1 = nbdl::make_delta<nbdl::delta_tag>(p2, p1); // _,        McGeez, 43
  auto d2 = nbdl::make_delta<nbdl::delta_tag>(p3, p2); // Scrappy,  Doo,    6
  auto d3 = nbdl::make_delta<nbdl::delta_tag>(p4, p3); // _,        _,      500
  auto d4 = nbdl::make_delta<nbdl::delta_tag>(p4, p4); // _,        _,      _

  CHECK(nbdl::apply_delta(d2, d1));
  CHECK(nbdl::apply_delta(d3, d1));
  CHECK(!nbdl::apply_delta(d4, d1));
  CHECK(nbdl::apply_delta(d1, p1));

  CHECK("Scrappy" == p1.name_first);
  CHECK("Doo"     == p1.name_last);
  CHECK(500       == p1.age);

  // check idempotence

  nbdl::apply_delta(d2, d1);
  nbdl::apply_delta(d3, d1);
  nbdl::apply_delta(d4, d1);
  CHECK(!nbdl::apply_delta(d1, p1));

  CHECK("Scrappy" == p1.name_first);
  CHECK("Doo"     == p1.name_last);
  CHECK(500       == p1.age);
}
