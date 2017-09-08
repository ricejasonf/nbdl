//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/context_store.hpp>
#include <nbdl/match_path.hpp>
#include <nbdl/message.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>

namespace hana    = boost::hana;

TEST_CASE("Make a store for use in nbdl::context", "[context_store][make_store]")
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  using my_store = nbdl::context_store<hana::tuple<int>>;

  auto store = nbdl::make_store<my_store>(hana::type_c<void>, hana::type_c<void>);

  auto check_type = hana::typeid_(store) == hana::type_c<nbdl::context_store_t<hana::tuple<int>>>;
  bool check_1 = false;
  bool check_2 = false;

  static_assert(decltype(check_type)::value, "Expecting type nbdl::context_store_t<hana::tuple<int>>.");

  nbdl::apply_action(store, hana::make_tuple(
    channel::downstream{}
  , action::update{}
  , hana::type_c<void>
  , hana::nothing
  , hana::nothing
  , hana::just(hana::make_tuple(42))
  ));

  nbdl::match(store, hana::type_c<void>, [&](auto const& value)
  {
    auto check_type = hana::typeid_(value) == hana::type_c<hana::tuple<int>>;
    static_assert(decltype(check_type)::value, "Expecting type hana::tuple<int>.");
    check_1 = hana::at_c<0>(value) == 42;
  });

  nbdl::match_path(store, hana::make_tuple(hana::type_c<void>, hana::type_c<int>), [&](auto value)
  {
    auto check_type = hana::typeid_(value) == hana::type_c<int>;
    static_assert(decltype(check_type)::value, "Expecting type int.");
    check_2 = value == 42;
  });

  CHECK(check_1);
  CHECK(check_2);
}
