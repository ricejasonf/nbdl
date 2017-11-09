//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/bind_map.hpp>
#include <nbdl/bind_sequence.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/match_path.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <catch.hpp>
#include <string>
#include <string_view>

namespace hana = boost::hana;

namespace entity_test
{
  struct person_t
  {
    std::string name;
    int age;
  };

  struct account_t
  {
    std::string username;
    person_t person;
  };
}

namespace nbdl
{
  NBDL_ENTITY(
    entity_test::person_t
    , name
    , age
  );

  NBDL_ENTITY(
    entity_test::account_t
    , username
    , person
  );
} // nbdl

TEST_CASE("Entity equality.", "[entity]") 
{
  using entity_test::person_t;
  person_t p1{"Skippy", 42};
  person_t p2{"Skippy", 42};
  person_t p3{"Skippy", 43};
  person_t p4{"Skippyz", 42};

  CHECK(hana::equal(p1, p2));
  CHECK(!hana::equal(p1, p3));
  CHECK(!hana::equal(p1, p4));
}

TEST_CASE("Entity bind_sequence", "[entity][bind_sequence]")
{
  using entity_test::person_t;
  bool result = nbdl::bind_sequence(person_t{"Skippy", 42}, [](auto&& ...xs)
  {
    return hana::equal(
      hana::make_tuple(std::forward<decltype(xs)>(xs)...),
      hana::make_tuple(std::string("Skippy"), 42)
    );
  });
  CHECK(result);
}

TEST_CASE("Entity bind_map", "[entity][bind_map]")
{
  using entity_test::person_t;
  bool result = nbdl::bind_map(person_t{"Skippy", 42}, [](auto&& ...pair)
  {
    return hana::equal(
      hana::make_tuple(
        hana::unpack(pair, [](auto&& first, auto&& second)
        {
          return hana::make_pair(first, second.get());
        })...
      ),
      hana::make_tuple(
        hana::make_pair(std::string_view{"name"}, "Skippy"),
        hana::make_pair(std::string_view{"age"}, 42)
      )
    );
  });
  CHECK(result);
}

TEST_CASE("Entity bind_map... again", "[entity][bind_map]")
{
  using entity_test::account_t;
  using entity_test::person_t;
  bool result = nbdl::bind_map(account_t{ "@skippy", person_t{"Skippy", 42}}, [](auto&& ...pair)
  {
    return hana::equal(
      hana::make_tuple(
        hana::unpack(pair, [](auto&& first, auto&& second)
        {
          return hana::make_pair(first, second.get());
        })...
      ),
      hana::make_tuple(
        hana::make_pair(std::string_view{"username"}, "@skippy"),
        hana::make_pair(std::string_view{"person"}, person_t{"Skippy", 42})
      )
    );
  });
  CHECK(result);
}

TEST_CASE("Entity as a Store.", "[entity][Store]")
{
  entity_test::account_t account{
    std::string{"@JasonRice_"}
  , entity_test::person_t{"Jason Rice", 22}
  };

  auto path1 = hana::make_tuple(
    NBDL_MEMBER(&entity_test::account_t::person){}
  , NBDL_MEMBER(&entity_test::person_t::name){}
  );

  // Would look better with literals
  auto path2 = hana::make_tuple(
    hana::string<'p', 'e', 'r', 's', 'o', 'n'>{}
  , hana::string<'n', 'a', 'm', 'e'>{}
  );

  std::string result1;
  std::string result2;

  nbdl::match_path(
    account, path1
  , [&](std::string const& name) { result1 = name; }
  , [](auto&&) { }
  );

  nbdl::match_path(
    account, path2
  , [&](std::string const& name) { result2 = name; }
  , [](auto&&) { }
  );

  CHECK(result1 == std::string{"Jason Rice"});
  CHECK(result2 == std::string{"Jason Rice"});
}
