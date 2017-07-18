//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/find_index.hpp>

#include <boost/hana/tuple.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

TEST_CASE("Find index first element in a tuple that satisfies a predicate.", "[detail][find_index]")
{
  auto eq = [](auto expected) { return [=](auto value) { return expected == value; }; };
  auto always = [](auto result) { return [=](auto) { return result; }; };
  auto preds = hana::make_tuple(eq(40), eq(41), eq(42), eq(43), always(hana::true_c));

  CHECK(nbdl::detail::find_index(preds, 40) == 0);
  CHECK(nbdl::detail::find_index(preds, 41) == 1);
  CHECK(nbdl::detail::find_index(preds, 42) == 2);
  CHECK(nbdl::detail::find_index(preds, 43) == 3);
  CHECK(nbdl::detail::find_index(preds, 44) == 4);
  CHECK(nbdl::detail::find_index(preds, 45) == 4);
}

TEST_CASE("Find index first element in empty tuple that satisfies a predicate.", "[detail][find_index]")
{
  auto always = [](auto result) { return [=](auto) { return result; }; };
  auto preds = hana::make_tuple(always(hana::true_c));

  CHECK(nbdl::detail::find_index(preds, 40) == 0);
  CHECK(nbdl::detail::find_index(preds, 41) == 0);
  CHECK(nbdl::detail::find_index(preds, 42) == 0);
  CHECK(nbdl::detail::find_index(preds, 43) == 0);
  CHECK(nbdl::detail::find_index(preds, 44) == 0);
  CHECK(nbdl::detail::find_index(preds, 45) == 0);
}
