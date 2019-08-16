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
#include <nbdl/app/context_endpoint.hpp>

#include <memory>

namespace nbdl::app {
  using full_duplex::promise;
  using full_duplex::do_;

  namespace client_detail {
    template <typename ContextTag, typename Client>
    auto make_connection(Client& c) {
      return client_endpoint_open(std::ref(c), endpoint(
        event::error          = log_error,
        event::read_message   = do_(deserializer_downstream<ContextTag>(),
                                    apply_read),
        event::terminate      = tap([&c](auto&&) {
          [](auto& /*lazy_c*/) {
            // TODO actually track connection attempts
            // just reconnect
            //lazy_c.connect(); 
          }(c);
        })
      ));
    }
  }

  struct client;

  template <typename Context>
  class client_impl {
    using context_tag = typename Context::tag;
    using client_connection_t = decltype(
        client_detail::make_connection<context_tag>(
          std::declval<client_impl&>()));

  public:
    using hana_tag = client;

    Context context;
    std::string address;
    client_connection_t conn;
    serializer_upstream<context_tag> serializer_ = {};

    client_impl(client_impl const&) = delete;

    template <typename Value>
    client_impl(actor_initializer<Context, Value> a)
      : context(a.context)
      , address(std::move(a.value))
    { }

    void connect() {
      conn = client_detail::make_connection<context_tag>(*this);
    }

    template <typename Message>
    void send_message(Message&& message) {
      if (!conn) {
        // extra safe here
        return;
      }

      conn->send_message(serializer_.serialize(std::move(message)));
    }
  };

  struct client {
    std::string address;

    template <typename Context>
    using actor_impl = client_impl<Context>;
  };
}

namespace nbdl {
  template <>
  struct producer_init_impl<app::client> {
    template <typename Producer>
    static void apply(Producer& p) {
      p.connect();
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
