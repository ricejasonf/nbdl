//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/catch.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/match.hpp>
#include <nbdl/path_promise.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/variant.hpp>

#include <algorithm>
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
  struct username_t { std::string value; };
  struct name_first_t { std::string value; };
  struct name_last_t { std::string value; };

  struct person_t
  {
    hana::tuple<name_first_t, name_last_t> name;
  };

  struct account_t
  {
    username_t username;
    nbdl::optional<person_t> person;
  };

  // used for example
  template <typename T>
  auto require_type = nbdl::promise([](auto& resolver, auto const& value)
  {
    if constexpr(decltype(hana::typeid_(value) == hana::type_c<T>){})
      resolver.resolve(value);
    else
      resolver.reject(value);
  });
}

namespace nbdl
{
  NBDL_ENTITY(
    person_t
    , name
  );

  NBDL_ENTITY(
    account_t
    , username
    , person
  );
} // nbdl

TEST_CASE("Promise to a nested value in a store", "[run_sync][params_promise]")
{
  using namespace nbdl::ui_spec;

  std::string result{"FAIL!"};

  account_t account{
    username_t{"@JasonRice_"}
  , person_t{hana::make_tuple(name_first_t{"Skippy"}, name_last_t{"McGee"})}
  };

  auto value_of = [](auto const& x) { return x.value; };
  auto to_upper = [](std::string str) { for (auto & c: str) c = ::toupper(c); return str; };

  nbdl::run_sync(
    nbdl::pipe(
      nbdl::path_promise(get("person"_s, "name"_s, hana::type_c<name_last_t>))
    , require_type<name_last_t>
    , value_of
    , to_upper
    , nbdl::tap([&](std::string const& str) { result = str; })
    , nbdl::catch_([](auto&&) { })
    )
  , account
  );

  // MCGEE

  CHECK(result == std::string{"MCGEE"});
}

TEST_CASE("Reject promise to a nested value in a store", "[run_sync][path_promise]")
{
  using namespace nbdl::ui_spec;

  std::string result{"FAIL!"};

  account_t account{
    username_t{"@JasonRice_"}
  , nbdl::nothing{}
  };

  auto value_of = [](auto const& x) { return x.value; };
  auto to_upper = [](std::string str) { for (auto & c: str) c = ::toupper(c); return str; };

  nbdl::run_sync(
    nbdl::pipe(
      nbdl::path_promise(get("person"_s, "name"_s, hana::type_c<name_last_t>))
    , require_type<name_last_t>
    , value_of
    , to_upper
    , nbdl::tap([](std::string const&) { })
    , nbdl::catch_([&](auto&&) { result = "something else"; })
    )
  , account
  );

  // something else

  CHECK(result == std::string{"something else"});
}
