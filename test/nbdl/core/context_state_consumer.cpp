//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/mock_store.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/map_store.hpp>
#include <nbdl/message.hpp>

#include <catch.hpp>

namespace hana = boost::hana;
namespace message = nbdl::message;
using test_context::path;
using test_context::entity::my_entity;

namespace test_context_state_consumer
{
  struct my_context { };
}

namespace nbdl
{
  template <>
  struct make_def_impl<test_context_state_consumer::my_context>
  {
    static constexpr auto apply()
    {
      return test_context_def::make(
        test_context::producer_tag{},
        test_context::producer_tag{},
        test_context::state_consumer_tag{},
        test_context::consumer_tag{},
        test_context::mock_store_tag{}
      );
    }
  };
}

namespace
{
  auto context = nbdl::make_unique_context<test_context_state_consumer::my_context>();
  auto& producer0       = context->actor<0>();
  auto& state_consumer  = context->actor<2>();
  auto& notifications   = state_consumer.recorded_notifications;
}

void init_mock_stuff()
{
  notifications = {};
  mock_store_result_apply_action = {};
  mock_store_result_apply_foreign_message = {};
}

TEST_CASE("No notifications are sent if state does not change.", "[context]") 
{
  init_mock_stuff();
  producer0.push_api.push(message::make_downstream_create(
    path<1>(1, 1)
  , message::no_uid
  , my_entity<1>{1, 1}
  , message::no_is_confirmed
  ));
  CHECK(notifications.size() == 0);
}

TEST_CASE("A single notification is sent when state changes for a single, primary store.", "[context]") 
{
  init_mock_stuff();
  mock_store_result_apply_action = path<1>(1, 1);
  producer0.push_api.push(message::make_downstream_create(
    path<1>(1, 1)
  , message::no_uid
  , my_entity<1>{1, 1}
  , message::no_is_confirmed
  ));
  REQUIRE(notifications.size() == 1);
  bool result = false;
  notifications[0].match(
    [&](path<1> const& p) { result = hana::equal(p, path<1>(1, 1)); },
    [](auto const&) { }
  );
  CHECK(result);
}

TEST_CASE("Notifications are sent when a store is listening to actions from other paths.", "[context]") 
{
  init_mock_stuff();
  hana::at_c<0>(mock_store_result_apply_foreign_message) = path<2>(2, 1); // path of store
  hana::at_c<1>(mock_store_result_apply_foreign_message) = path<1>(1, 1); // trigger
  hana::at_c<2>(mock_store_result_apply_foreign_message).push_back(path<2>(2, 1)); // resulting notify
  hana::at_c<2>(mock_store_result_apply_foreign_message).push_back(path<2>(2, 2)); // resulting notify
  producer0.push_api.push(message::make_downstream_create(
    path<1>(1, 1)
  , message::no_uid
  , my_entity<1>{1, 1}
  , message::no_is_confirmed
  ));
  REQUIRE(notifications.size() == 2);

  bool result1 = false;
  notifications[0].match(
    [&](path<2> const& p) { result1 = hana::equal(p, path<2>(2, 1)); },
    [](auto const&) { }
  );

  bool result2 = false;
  notifications[1].match(
    [&](path<2> const& p) { result2 = hana::equal(p, path<2>(2, 2)); },
    [](auto const&) { }
  );

  CHECK(result1);
  CHECK(result2);
}
