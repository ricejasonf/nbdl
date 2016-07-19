//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_map.hpp>
#include <nbdl/bind_sequence.hpp>
#include <nbdl/entity.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

namespace entity_test
{
  struct person
  {
    std::string name;
    int age;
  };
}

namespace nbdl
{
  NBDL_ENTITY(
    entity_test::person
    , name
    , age
  );
} // nbdl

TEST_CASE("Entity equality.", "[entity]") 
{
  using entity_test::person;
  person p1{"Skippy", 42};
  person p2{"Skippy", 42};
  person p3{"Skippy", 43};
  person p4{"Skippyz", 42};

  CHECK(hana::equal(p1, p2));
  CHECK(!hana::equal(p1, p3));
  CHECK(!hana::equal(p1, p4));
}

TEST_CASE("Entity bind_sequence", "[entity][bind_sequence]")
{
  using entity_test::person;
  bool result = nbdl::bind_sequence(person{"Skippy", 42}, [](auto&& ...xs)
  {
    return hana::equal(
      hana::make_tuple(std::forward<decltype(xs)>(xs)...),
      hana::make_tuple(std::string("Skippy"), 42)
    );
  });
  CHECK(result);
}

// TODO determine if bind_map should return a value like a fold expression
TEST_CASE("Entity bind_map", "[entity][bind_map]")
{
  using entity_test::person;
  bool result = nbdl::bind_map(person{"Skippy", 42}, [](auto&& ...pair)
  {
    return hana::equal(
      hana::make_tuple(
        hana::unpack(pair, [](auto&& first, auto&& second)
        {
          return hana::make_pair(first, second.get());
        })...
      ),
      hana::make_tuple(
        hana::make_pair("name", "Skippy"),
        hana::make_pair("age", 42)
      )
    );
  });
  CHECK(result);
}
