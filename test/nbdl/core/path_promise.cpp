//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/catch.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/path_promise.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

#define MAKE_STRING(NAME) constexpr auto NAME##_s = BOOST_HANA_STRING(#NAME);

namespace
{
  struct name_first { std::string value; };
  struct name_last { std::string value; };

  using name_t = hana::tuple<name_first, name_last>;

  struct person_t
  {
    name_t name;
  };

  struct account_t
  {
    std::string username;
    nbdl::optional<person_t> person;
  };

  using account_store = nbdl::variant<account_t>;

  // pseudo compile-time string literals

  MAKE_STRING(person);
  MAKE_STRING(name);
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


TEST_CASE("path_promise to match on Store element as a promise.", "[path_promise][Store]")
{
  constexpr auto path_spec = nbdl::ui_spec::path_t<
    nbdl::ui_spec::get_t<decltype(name_s), hana::type<name_last>>
  , nbdl::ui_spec::match_type<person_t, decltype(person_s)>
  , nbdl::ui_spec::match_type<account_t>
  >{};

  auto my_promise = nbdl::path_promise(path_spec);

  {
    std::string result;

    account_store account{account_t{
      std::string{"@JasonRice_"}
    , person_t{hana::make_tuple(name_first{"Jason"}, name_last{"Rice"})}
    }};

    nbdl::run_sync(nbdl::pipe(
      hana::always(account)
    , my_promise
    , nbdl::tap([&](name_last const& x) { result = x.value; })
    , nbdl::catch_([&](auto&&) { result = "FAIL!"; })
    ));

    CHECK(result == std::string{"Rice"});
  }
  {
    std::string result;

    account_store account{account_t{
      std::string{"@JasonRice_"}
    , nbdl::nothing{}
    }};

    nbdl::run_sync(nbdl::pipe(
      hana::always(account)
    , my_promise
    , nbdl::catch_([&](nbdl::nothing) { result = "OK!"; })
    , nbdl::catch_([&](auto&&) { result = "FAIL!"; })
    ));

    CHECK(result == std::string{"OK!"});
  }
  {
    std::string result;

    account_store account{};

    nbdl::run_sync(nbdl::pipe(
      hana::always(account)
    , my_promise
    , nbdl::catch_([&](nbdl::unresolved) { result = "OK!"; })
    , nbdl::catch_([&](auto&&) { result = "FAIL!"; })
    ));

    CHECK(result == std::string{"OK!"});
  }
}
