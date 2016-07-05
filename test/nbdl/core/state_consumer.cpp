//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <assets/TestContext.hpp>

#include <catch.hpp>

constexpr auto def = test_context_def::make(
  test_context::provider_tag{},
  test_context::provider_tag{},
  test_context::state_consumer_tag{},
  test_context::consumer_tag{},
  nbdl::map_store_tag{}
);
auto context = nbdl::make_unique_context(def);

auto& provider0       = context->cell<0>();
auto& state_consumer  = context->cell<3>();

auto& init_recorded_notifications()
{
  state_consumer.recorded_notifications = {};
  return state_consumer.recorded_notifications;
}

TEST_CASE("Initial read should not change state.", "[context]") 
{
  auto& notifications = init_recorded_notifications();
  provider0.push(provider0.push_api.make_downstream_create_message(
      test_context::path<1>(1, 1),
      entity::my_entity<1>{1, 1}
  ));
  REQUIRE(notifications.length() == 1);
  CHECK(hana::equal(notifications[0], test_context::path<1>(1, 1)));
}

TEST_CASE("Initial create should not change state.", "[context]") 
{
  auto& notifications = init_recorded_notifications();
  provider0.push(provider0.push_api.make_downstream_create_message(
      test_context::path<1>(1, 1),
      entity::my_entity<1>{1, 1}
  ));
  REQUIRE(notifications.length() == 1);
  CHECK(hana::equal(notifications[0], test_context::path<1>(1, 1)));
}
