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

TEST_CASE("Dispatch Upstream/Downstream Read Messages", "[context]")
{
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
    return consumer.recorded_messages[0].match(
      [](nbdl::unresolved) { return false; },
      [](test_context::null_system_message) { return false; },
      [](auto const& m)
        -> std::enable_if_t<nbdl::DownstreamMessage<decltype(m)>::value,
          decltype((*message::get_maybe_payload(m)).id == 2)>
      {
        auto const& payload = *message::get_maybe_payload(m);
        CHECK(payload.id == 2);
        CHECK(payload.root_id == 1);
        bool path_check = hana::equal(
          message::get_path(m), 
          test_context::path1(1, 2)
        );
        CHECK(path_check);
        return true;
      },
      [](auto const&)
      {
        return false;
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

  auto check_upstream_read = [](auto const& provider)
  {
    return provider.recorded_messages[0].match(
      [](auto const& m)
        -> std::enable_if_t<nbdl::UpstreamMessage<decltype(m)>::value, bool>
      {
        bool path_check = hana::equal(
          message::get_path(m), 
          test_context::path1(1, 2)
        );
        CHECK(path_check);
        return true;
      },
      [](auto const&)
      {
        return false;
      }
    );
  };

  // provider0 should record an upstream read message.
  CHECK(check_upstream_read(provider0));
}
