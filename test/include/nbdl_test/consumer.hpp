//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_CONSUMER_HPP
#define NBDL_TEST_CONSUMER_HPP

#include <functional>
#include <nbdl/actor_type.hpp>
#include <nbdl/send_downstream_message.hpp>

namespace nbdl_test {
  namespace hana = boost::hana;

  template <typename Context>
  struct consumer_impl;

  struct consumer { 
    template <typename Context>
    using actor_impl = consumer_impl<Context>;
  };

  template <typename Context>
  struct consumer_impl {
    using hana_tag = consumer;

    template <typename Value>
    consumer_impl(nbdl::actor_initializer<Context, Value> a)
      : context(a.context)
      , on_message(a.value)
    { }

    Context context;
    std::function<void(std::string)> on_message;
  };
}

namespace nbdl {
#if 0
  template <typename Tag, bool condition>
  struct consumer_init_impl<Tag, nbdl_test::consumer>
  {
    template <typename Consumer>
    static constexpr void apply(Consumer& c) {
      c.init(c);
    }
  };
#endif

  template <>
  struct send_downstream_message_impl<nbdl_test::consumer> {
    template <typename Consumer, typename Message>
    static constexpr void apply(Consumer const& c, Message&& m) {
      static_assert(
        std::is_same_v<
            std::string,
            decltype(nbdl::message::get_payload(std::forward<Message>(m)))>,
        "nbdl_test::consumer expects an std::string payload"
      );
      c.on_message(nbdl::message::get_payload(std::forward<Message>(m)));
    }
  };
}

#endif
