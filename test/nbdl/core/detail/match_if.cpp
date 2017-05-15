//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/match_if.hpp>

#include <boost/hana/functional/always.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <mpdef/pair.hpp>
#include <string>

namespace hana = boost::hana;
using namespace hana::literals;

TEST_CASE("Match first element in a tuple that satisfies a predicate.", "[detail][match_if]")
{
  auto eq = [](std::size_t expected) { return [=](std::size_t value) { return expected == value; }; };
  auto always = [](auto result) { return [=](auto) { return result; }; };
  auto preds = hana::make_tuple(eq(40), eq(41), eq(42), eq(43), always(hana::true_c));

  auto run = [&](std::size_t value)
  {
    std::size_t result = 9000;
    nbdl::run_sync(
      nbdl::pipe(
        nbdl::detail::match_if(preds)
      , [&](auto index) { result = index; }
      )
    , value
    );
    return result;
  };

  CHECK(run(40) == 0);
  CHECK(run(41) == 1);
  CHECK(run(42) == 2);
  CHECK(run(43) == 3);
  CHECK(run(44) == 4);
  CHECK(run(45) == 4);
}
