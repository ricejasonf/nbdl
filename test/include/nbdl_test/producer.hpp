//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_PRODUCER_HPP
#define NBDL_TEST_PRODUCER_HPP

#include <nbdl/actor_type.hpp>
#include <nbdl/send_upstream_message.hpp>

namespace nbdl_test {
  namespace hana = boost::hana;

  struct producer { };

  template <typename Context, typename OnMessage>
  struct producer_impl {
    using hana_tag = producer;

    producer_impl(nbdl::actor_initializer<Context, producer> a)
      : context(a.context)
      , on_message(a.value)
    { }

    Context context;
    OnMessage on_message;
  };
}

namespace nbdl {
  template <>
  struct send_upstream_message_impl<nbdl_test::producer> {
    template <typename Consumer, typename Message>
    static constexpr void apply(Consumer const& c, Message&& m) {
      c.on_message(std::forward<Message>(m));
    }
  };
}

#endif
