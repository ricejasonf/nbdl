//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <catch.hpp>
#include <nbdl/consumer_init.hpp>
#include <nbdl/context.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/producer_init.hpp>
#include <nbdl/send_downstream_message.hpp>
#include <nbdl/send_upstream_message.hpp>
#include <string_view>
#include <vector>

namespace {
  using event_list = std::vector<std::string_view>;
  event_list test_events{};
}

namespace test {
  struct producer {
    template <typename T>
    producer(T&&) { }

    template <typename>
    using actor_impl = producer;
  };
  struct consumer {
    template <typename T>
    consumer(T&&) { }

    template <typename>
    using actor_impl = producer;
  };
}

namespace nbdl {
  template <>
  struct send_upstream_message_impl<test::producer> {
    template <typename Producer, typename Message>
    static void apply(Producer&, Message&&) { }
  };

  template <>
  struct producer_init_impl<test::producer> {
    template <typename Producer>
    static void apply(Producer&) {
      test_events.push_back("producer_init");
    }
  };

  template <>
  struct send_downstream_message_impl<test::consumer> {
    template <typename Consumer, typename Message>
    static void apply(Consumer&, Message&&) { }
  };

  template <>
  struct consumer_init_impl<test::consumer> {
    template <typename Consumer>
    static void apply(Consumer&) {
      test_events.push_back("consumer_init");
    }
  };
}

namespace test {
  struct context_tag {
    constexpr static auto make_def() {
      using namespace nbdl_def;
      return Context(
        Producers(
          Producer(
            Name("test_producer"),
            Type<producer>
          )
        ),
        Consumers(
          Consumer(
            Name("test_consumer"),
            Type<consumer>
          )
        )
      );
    }
  };
}

TEST_CASE("Context should init producers then consumers") {
  auto ctx = nbdl::make_context<test::context_tag>(
    nbdl::actor("test_producer", 42),
    nbdl::actor("test_consumer", 42)
  );

  test::producer foo = ctx->actor<0>();
  test::consumer bar = ctx->actor<1>();

  event_list expected = {"producer_init", "consumer_init"};
  CHECK(test_events == expected);
}
