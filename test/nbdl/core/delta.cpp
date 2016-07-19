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
} // nbdl

TEST_CASE("make_delta", "[delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{"Skippy", "McGeez", 43};

  auto d = nbdl::make_delta<nbdl::delta_tag>(p2, p1);

  hana::unpack(d.storage, [&](auto const& maybe_name_first,
                              auto const& maybe_name_last,
                              auto const& maybe_age
                             )
  {
    bool result1 = maybe_name_first.match(
      [](nbdl::nothing) { return true; },
      [](auto&&)        { return false; }
    );
    bool result2 = maybe_name_last.match(
      [](std::string const& name_last)  { return name_last == "McGeez"; },
      [](nbdl::nothing)                 { return false; }
    );
    bool result3 = maybe_age.match(
      [](int age)       { return age = 43; },
      [](nbdl::nothing) { return false; }
    );
    CHECK(result1);
    CHECK(result2);
    CHECK(result3);
  });
}

TEST_CASE("apply_delta mutate object in place", "[delta][apply_delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{"Skippy", "McGeez", 43};
  person p3{"Scrappy", "Doo", 6};

  auto d = nbdl::make_delta<nbdl::delta_tag>(p2, p1);

  nbdl::apply_delta(d, p3);

  CHECK("Scrappy" == p3.name_first);
  CHECK("McGeez"  == p3.name_last);
  CHECK(43        == p3.age);
}

TEST_CASE("apply_delta using rvalue", "[delta][apply_delta]")
{
  using delta_test_entity::person;
  person p1{"Skippy", "McGee", 42};
  person p2{"Skippy", "McGeez", 43};

  auto d = nbdl::make_delta<nbdl::delta_tag>(p2, p1);

  person p3 = nbdl::apply_delta(d, person{"Scrappy", "Doo", 6});

  CHECK("Scrappy" == p3.name_first);
  CHECK("McGeez"  == p3.name_last);
  CHECK(43        == p3.age);
}
