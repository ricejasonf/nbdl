//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/message.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/value.hpp>
#include <catch.hpp>

namespace hana    = boost::hana;
namespace entity  = test_context::entity;
namespace message = nbdl::message;
namespace channel = nbdl::message::channel;
namespace action  = nbdl::message::action;

constexpr auto def = test_context_def::make(
  test_context::provider_tag{},
  test_context::provider_tag{},
  test_context::consumer_tag{},
  test_context::consumer_tag{}
);
auto context = nbdl::make_unique_context(def);

auto& provider0 = context->cell<0>();
auto& provider1 = context->cell<1>();
auto& consumer2 = context->cell<2>();
auto& consumer3 = context->cell<3>();

void init_record_messages()
{
  provider0.recorded_messages = {};
  provider1.recorded_messages = {};
  consumer2.recorded_messages = {};
  consumer3.recorded_messages = {};
}

template <typename Channel, typename Action>
struct check_message_fn
{
  template <typename Message, typename Fn>
  constexpr auto operator()(Message const& m, Fn const& fn) const
  {
    return m.match(
      [](nbdl::unresolved) { return false; },
      [](test_context::null_system_message) { return false; },
      [&](auto const& m)
        -> std::enable_if_t<decltype(
              hana::type_c<const Channel> == hana::decltype_(message::get_channel(m))
          &&  hana::type_c<const Action>  == hana::decltype_(message::get_action(m))
          )::value, decltype(fn(m))>
      {
        return fn(m);
      },
      [](auto const&) { return false; }
    );
  }
};

template <typename Channel, typename Action>
constexpr check_message_fn<Channel, Action> check_message{};

template <typename Message>
void check_path(Message const& m)
{
  CHECK(hana::equal(
    message::get_path(m), 
    test_context::path1(1, 2)
  ));
}

template <typename Message>
void check_payload(Message const& m)
{
  auto const& payload = *message::get_maybe_payload(m);
  CHECK(payload.id == 2);
  CHECK(payload.root_id == 1);
}

template <typename Message>
constexpr auto has_payload(Message const&)
{
  return decltype(hana::equal(
    hana::maybe(
      hana::type_c<void>,
      hana::decltype_,
      message::get_maybe_payload(std::declval<Message>())
    ),
    hana::type_c<entity::my_entity>
  )){};
}

TEST_CASE("Dispatch Upstream/Downstream Read Messages", "[context]")
{
  init_record_messages();

  // Send downstream read to consumers.
  provider0.push_api.push(
    provider0.push_api.make_downstream_read_message(
      test_context::path1(1, 2),
      entity::my_entity{2, 1}
    )
  );
  CHECK(consumer2.recorded_messages.size() == 1);
  CHECK(consumer3.recorded_messages.size() == 1);

  auto check_downstream_read = [](auto const& consumer)
  {
    return check_message<channel::downstream, action::read>(consumer.recorded_messages[0],
      [](auto const& m)
        -> std::enable_if_t<decltype(has_payload(m))::value, bool>
      {
        check_path(m);
        check_payload(m);
        return true;
      }
    );
  };

  // Both consumers got the downstream
  // read message from provider0
  CHECK(check_downstream_read(consumer2));
  CHECK(check_downstream_read(consumer3));

  // Send upstream read to provider0.
  consumer2.push_api.push(
    consumer2.push_api.make_upstream_read_message(test_context::path1(1, 2))
  );
  CHECK(provider0.recorded_messages.size() == 1);
  // provider1 should not receive the message.
  CHECK(provider1.recorded_messages.size() == 0);

  bool result = check_message<channel::upstream, action::read>(provider0.recorded_messages[0],
    [](auto const& m)
    {
      check_path(m);
      return true;
    }
  );

  // provider0 should record an upstream read message.
  CHECK(result);
}

TEST_CASE("Dispatch Upstream/Downstream Create Messages", "[context]")
{
  init_record_messages();

  // Send downstream create to consumers.
  provider0.push_api.push(
    provider0.push_api.make_downstream_create_message(
      test_context::path1(1, 2),
      entity::my_entity{2, 1}
    )
  );
  CHECK(consumer2.recorded_messages.size() == 1);
  CHECK(consumer3.recorded_messages.size() == 1);

  auto check_downstream_create = [](auto const& consumer)
  {
    return check_message<channel::downstream, action::create>(consumer.recorded_messages[0],
      [](auto const& m)
        -> std::enable_if_t<decltype(has_payload(m))::value, bool>
      {
        check_path(m);
        check_payload(m);
        return true;
      }
    );
  };

  // Both consumers got the downstream
  // create message from provider0
  CHECK(check_downstream_create(consumer2));
  CHECK(check_downstream_create(consumer3));

  // Send upstream create to provider0.
  consumer2.push_api.push(
    consumer2.push_api.make_upstream_create_message(
      test_context::path1(1, 2),
      entity::my_entity{2, 1}
    )
  );
  CHECK(provider0.recorded_messages.size() == 1);
  // provider1 should not receive the message.
  CHECK(provider1.recorded_messages.size() == 0);

  bool result = check_message<channel::upstream, action::create>(provider0.recorded_messages[0],
    [](auto const& m)
        -> std::enable_if_t<decltype(has_payload(m))::value, bool>
    {
      check_path(m);
      check_payload(m);
      return true;
    }
  );

  // provider0 should record an upstream create message.
  CHECK(result);
}
