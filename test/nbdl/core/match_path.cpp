//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/entity.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

#define MAKE_STRING(NAME) auto NAME##_s = BOOST_HANA_STRING(#NAME);

namespace
{
  struct name_first { std::string value; };
  struct name_last { std::string value; };

  struct person_t
  {
    hana::tuple<name_first, name_last> name;
  };

  struct account_t
  {
    std::string username;
    nbdl::optional<person_t> person;
  };


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


TEST_CASE("match_path on a deeply nested tree of Stores.", "[match_path][Store]")
{
  {
    std::string result;

    account_t account{
      std::string{"@JasonRice_"}
    , person_t{hana::make_tuple(name_first{"Jason"}, name_last{"Rice"})}
    };

    nbdl::match_path(
      account
    , hana::make_tuple(person_s, name_s, hana::type_c<name_last>) 
    , [&](name_last const& x) { result = x.value; }
    , [](auto&&) { }
    );

    CHECK(result == std::string{"Rice"});
  }
  {
    std::string result;

    account_t account{
      std::string{"@JasonRice_"}
    , nbdl::nothing{}
    };

    nbdl::match_path(
      account
    , hana::make_tuple(person_s, name_s, hana::type_c<name_last>) 
    , [&](name_last const& x) { result = x.value; }
    , [](auto&&) { }
    );

    CHECK(result == std::string{""});
  }
}
