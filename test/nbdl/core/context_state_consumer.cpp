//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/map_store.hpp>

#include <catch.hpp>

namespace hana = boost::hana;
using test_context::path;
using test_context::entity::my_entity;

constexpr auto def = test_context_def::make(
  test_context::provider_tag{},
  test_context::provider_tag{},
  test_context::state_consumer_tag{},
  test_context::consumer_tag{},
  test_context::mock_store_tag{}
);
namespace
{
  auto context = nbdl::make_unique_context(def);
  auto& provider0       = context->cell<0>();
  auto& state_consumer  = context->cell<2>();
  auto& notifications   = state_consumer.recorded_notifications;
}

void init_mock_stuff()
{
  notifications = {};
  mock_store_result_apply_action = {};
  mock_store_result_apply_foreign_action = {};
}

TEST_CASE("No notifications are sent if state does not change.", "[context]") 
{
  init_mock_stuff();
  provider0.push_api.push(provider0.push_api.make_downstream_create_message(
      path<1>(1, 1),
      my_entity<1>{1, 1}
  ));
  CHECK(notifications.size() == 0);
}

TEST_CASE("A single notification is sent when state changes for a single, primary store.", "[context]") 
{
  init_mock_stuff();
  mock_store_result_apply_action = path<1>(1, 1);
  provider0.push_api.push(provider0.push_api.make_downstream_create_message(
      path<1>(1, 1),
      my_entity<1>{1, 1}
  ));
  REQUIRE(notifications.size() == 1);
  bool result = notifications[0].match(
    [](path<1> const& p) { return hana::equal(p, path<1>(1, 1)); },
    [](auto const&) { return false; }
  );
  CHECK(result);
}

TEST_CASE("A notification is sent when a store is listening to actions from other paths.", "[context]") 
{
  init_mock_stuff();
  hana::at_c<0>(mock_store_result_apply_foreign_action) = path<2>(2, 1); // path of store
  hana::at_c<1>(mock_store_result_apply_foreign_action) = path<1>(1, 1); // trigger
  hana::at_c<2>(mock_store_result_apply_foreign_action).push_back(path<2>(2, 1)); // resulting notify
  hana::at_c<2>(mock_store_result_apply_foreign_action).push_back(path<2>(2, 2)); // resulting notify
  provider0.push_api.push(provider0.push_api.make_downstream_create_message(
      path<1>(1, 1),
      my_entity<1>{1, 1}
  ));
  REQUIRE(notifications.size() == 2);
  bool result1 = notifications[0].match(
    [](path<2> const& p) { return hana::equal(p, path<2>(2, 1)); },
    [](auto const&) { return false; }
  );
  bool result2 = notifications[1].match(
    [](path<2> const& p) { return hana::equal(p, path<2>(2, 2)); },
    [](auto const&) { return false; }
  );
  CHECK(result1);
  CHECK(result2);
}

// TODO: test the multiple calls to apply_foreign_action
