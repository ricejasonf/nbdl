//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/entity.hpp>
#include <nbdl/match.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;
using namespace hana::literals;

namespace
{
  struct name_first_t { std::string value; };
  struct name_last_t { std::string value; };

  struct person_t
  {
    std::string name_first;
    std::string name_last;
  };
}

namespace nbdl
{
  NBDL_ENTITY(
    person_t
    , name_first
    , name_last
  );
}

TEST_CASE("Match on values in a hana::map.", "[match][Store]")
{
  std::string result("FAIL!");

  auto store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("Hello, World!"))
  , hana::make_pair("key_2"_s, 42)
  );

  nbdl::match(
    store
  , "key_1"_s
  , [&](std::string const& x) { result = x; }
  );

  // Hello, World!

  CHECK(result == std::string{"Hello, World!"});
}

TEST_CASE("Match on values in a nbdl::Entity.", "[match][Store]")
{
  std::string result("FAIL!");

  auto store = person_t{"John", "Smith"};

  nbdl::match(
    store
  , "name_first"_s
  , [&](std::string const& x) { result = x; }
  );

  // John

  CHECK(result == std::string{"John"});
}

TEST_CASE("Match on values in a hana::tuple.", "[match][Store]")
{
  std::string result("FAIL!");

  auto name_first = hana::type_c<name_first_t>;

  auto store = hana::make_tuple(name_first_t{"John"}, name_last_t{"Smith"});

  nbdl::match(
    store
  , name_first
  , [&](name_first_t const& x) { result = x.value; }
  );

  // John

  CHECK(result == std::string{"John"});
}

TEST_CASE("Match on values in a variant.", "[match][Store]")
{
  std::string result("FAIL!");

  nbdl::optional<std::string> store = std::string("Hello, maybe!");

  nbdl::match(
    store
  , [&](auto const& x)
    {
      if constexpr(hana::is_a<std::string, decltype(x)>)
        result = x;
    }
  );

  // Hello, maybe!

  CHECK(result == std::string{"Hello, maybe!"});
}

TEST_CASE("Match on values nested in a variant.", "[match][Store]")
{
  std::string result("FAIL!");

  nbdl::optional<person_t> store = person_t{"John", "Smith"};

  nbdl::match(
    store
  , "name_first"_s
  , [&](auto const& x)
    {
      if constexpr(hana::is_a<std::string, decltype(x)>)
        result = x;
    }
  );

  // John

  CHECK(result == std::string{"John"});
}
