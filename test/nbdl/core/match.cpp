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
#include <boost/hana/pair.hpp>
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

TEST_CASE("Match an arbitrary object.", "[match][Store]")
{
  std::string result("FAIL!");

  std::string store("Hello, string!");

  nbdl::match(
    std::move(store)
  , [&](std::string&& x) { result = std::move(x); }
  );

  // Hello, World!

  CHECK(result == std::string{"Hello, string!"});
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

TEST_CASE("Match on values in a hana::tuple.", "[core]")
{
  std::string result_1("FAIL!");
  std::string result_2("FAIL!");

  auto store = hana::make_tuple(name_first_t{"John"}, name_last_t{"Smith"});

  nbdl::match(
    store
  , hana::type<name_first_t>{}
  , [&](name_first_t const& x) { result_1 = x.value; }
  );

  nbdl::match(
    store
  , hana::type<name_last_t>{}
  , [&](name_last_t const& x) { result_2 = x.value; }
  );

  CHECK(result_1 == std::string{"John"});
  CHECK(result_2 == std::string{"Smith"});
}

TEST_CASE("Match on values in a hana::Product.", "[core]")
{
  std::string result_1("FAIL!");
  std::string result_2("FAIL!");

  auto store = hana::make_pair(name_first_t{"John"}, name_last_t{"Smith"});

  nbdl::match(
    store
  , hana::first
  , [&](name_first_t const& x) { result_1 = x.value; }
  );

  nbdl::match(
    store
  , hana::second
  , [&](name_last_t const& x) { result_2 = x.value; }
  );

  CHECK(result_1 == std::string{"John"});
  CHECK(result_2 == std::string{"Smith"});
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

TEST_CASE("Match with visitor match_when", "[match]")
{
  auto store = hana::make_map(
    hana::make_pair(hana::type_c<int>, 5)
  , hana::make_pair(hana::type_c<char>, 'x')
  );

  {
    bool result = false;
    nbdl::match_when<int>(
      store
    , hana::type_c<int>
    , [&](auto x)
      {
        CHECK(x == 5);
        result = true;
      }
    );

    CHECK(result);
  }

  {
    bool result = false;
    nbdl::match(
      std::move(store)
    , hana::type_c<int>
    , nbdl::match_when<void>([&](auto&&) { result = true; })
    );

    CHECK(not result);
  }
}

TEST_CASE("Match nbdl::variant with visitor match_when", "[match]")
{
  auto store = nbdl::variant<int, float, double, char>{int{42}};

  {
    bool result = false;

    nbdl::match(
      store
    , nbdl::match_when<int>([&](auto x)
      {
        CHECK(x == 42);
        result = true;
      })
    );

    CHECK(result);
  }

  {
    bool result = false;

    nbdl::match_when<float>(
      store
    , [&](auto) { result = true; }
    );

    CHECK(not result);
  }

  { // something not on the list
    bool result = false;

    nbdl::match(
      std::move(store)
    , nbdl::match_when<void>([&](auto&&) { result = true; })
    );

    CHECK(not result);
  }
}

TEST_CASE("Match with vistor mapped_overload", "[match]")
{
  auto store = hana::make_map(
    hana::make_pair(hana::type_c<int>, int{82})
  , hana::make_pair(hana::type_c<char>, 'x')
  );

  {
    std::string result = "FAIL";

    nbdl::match(
      store
    , hana::type_c<int>
    , nbdl::mapped_overload(
        hana::make_map(
          hana::make_pair(hana::type_c<int>, [&](auto x)
          {
            CHECK(x == 82);
            result = "int";
          })
        , hana::make_pair(hana::type_c<float>, [&](auto) { result = "float"; })
        , hana::make_pair(hana::type_c<double>, [&](auto) { result = "double"; })
        )
      , [&](auto) { result = "otherwise"; }
      )
    );

    CHECK(result == "int");
  }

  {
    std::string result = "FAIL";

    nbdl::match(
      store
    , nbdl::mapped_overload(
        hana::make_map(
          hana::make_pair(hana::type_c<float>, [&](auto) { result = "float"; })
        , hana::make_pair(hana::type_c<double>, [&](auto) { result = "double"; })
        )
      , [&](auto) { result = "otherwise"; }
      )
    );

    CHECK(result == "otherwise");
  }
}

TEST_CASE("Match variant with vistor mapped_overload", "[match]")
{
  auto store = nbdl::variant<int, float, double, char>{int{101}};

  {
    std::string result = "FAIL";

    nbdl::match(
      store
    , nbdl::mapped_overload(
        hana::make_map(
          hana::make_pair(hana::type_c<int>, [&](auto x)
          {
            CHECK(x == 101);
            result = "int";
          })
        , hana::make_pair(hana::type_c<float>, [&](auto) { result = "float"; })
        , hana::make_pair(hana::type_c<double>, [&](auto) { result = "double"; })
        )
      , [&](auto) { result = "otherwise"; }
      )
    );

    CHECK(result == "int");
  }

  {
    std::string result = "FAIL";

    nbdl::match(
      store
    , nbdl::mapped_overload(
        hana::make_map(
          hana::make_pair(hana::type_c<float>, [&](auto) { result = "float"; })
        , hana::make_pair(hana::type_c<double>, [&](auto) { result = "double"; })
        )
      , [&](auto) { result = "otherwise"; }
      )
    );

    CHECK(result == "otherwise");
  }
}
