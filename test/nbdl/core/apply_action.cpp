//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/apply_action.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

TEST_CASE("Apply action to hana::Searchable.", "[apply_action]")
{
  auto store = hana::make_tuple(5);

  nbdl::apply_action(store, hana::make_tuple(42));

  CHECK(hana::make_tuple(42) == store);
}
