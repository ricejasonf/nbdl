//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CLIENT_HPP
#define NBDL_CLIENT_HPP

#include <nbdl.hpp>
#include <nbdl/app/client_connection.hpp>
#include <nbdl/js.hpp>

namespace nbdl::app {
  struct client {
    std::string address;
  };

  template <typename Context>
  struct client_impl {
    using ContextTag = typename Context::tag;

    Context context;
    std::string address;

    // TODO
    void start_connecting() {
    }
  };
}

namespace nbdl {
  template <>
  struct producer_init_impl<app::client> {
    template <typename Producer>
    static void apply(Producer& p) {
      p.start_connecting();
    }
  };

  template <typename Context>
  struct actor_type<app::client, Context> {
    using type = nbdl::app::client_impl<Context>;
  };

  template <>
  struct send_upstream_message_impl<app::client> {
    template <typename Producer, typename Message>
    static void apply(Producer& p, Message&& m) {
      p.send_message(std::forward<Message>(m));
    }
  };
}

#endif
