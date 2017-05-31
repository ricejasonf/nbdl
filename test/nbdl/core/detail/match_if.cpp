//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/detail/match_if.hpp>

#include <boost/hana/bool.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

namespace
{
  template <std::size_t i>
  struct eq_fn
  {
    bool operator()(std::size_t value) const
    {
      return value == i;
    }
  };

  struct always_true_fn
  {
    constexpr hana::true_ operator()(std::size_t) const
    { return {}; }
  };
}

TEST_CASE("Match first element in a tuple that satisfies a predicate.", "[detail][match_if]")
{
  auto preds = mpdef::make_list(eq_fn<40>{}, eq_fn<41>{}, eq_fn<42>{}, eq_fn<43>{}, always_true_fn{});

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
