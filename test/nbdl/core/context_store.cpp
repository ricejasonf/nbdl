//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/make_store.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>

namespace hana    = boost::hana;

TEST_CASE("Make a store for use in nbdl::context", "[context_store][make_store]")
{
  using my_store = nbdl::context_store<hana::tuple<int>>;

  auto store = nbdl::make_store<my_store>(hana::type_c<void>, hana::type_c<void>);

  auto check_type = hana::typeid_(store) == hana::type_c<hana::tuple<int>>;
  static_assert(decltype(check_type)::value, "Store must equal specified type.");
  CHECK(hana::at_c<0>(store) == 0);
}
