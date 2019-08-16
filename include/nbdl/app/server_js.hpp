//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVER_JS_HPP
#define NBDL_SERVER_JS_HPP

// Consumer that manages messaging to socket clients
// FOR JAVASCRIPT LAND!! ;P ;P ;P

#include <nbdl/app/serializer.hpp>

#include <full_duplex.hpp>
#include <functional>
#include <nbdl.hpp>
#include <nbdl/js.hpp>
#include <vector>

namespace nbdl::app {
  // server - The construction arg for make_context
  struct server {
    unsigned short port;
  };

  template <typename Context>
  struct server_impl {
    using hana_tag = server;
    using ContextTag = typename Context::tag;

    Context context_;
    nbdl::js::callback_t add_connection_cb;
    nbdl::js::callback_t on_message_cb;
    nbdl::js::val ws_val = {};
    serializer_downstream<ContextTag> serializer_ = {};
    std::vector<nbdl::js::val> connections = {};
    unsigned short port;

    server_impl(server_impl const&) = delete;

    server_impl(nbdl::actor_initializer<Context, server> a)
      : context_(a.context)
      , add_connection_cb(nbdl::js::callback([this](nbdl::js::val x) {
          connections.push_back(std::move(x));
        }))
      , on_message_cb(nbdl::js::callback([this](nbdl::js::val x) {
          receive_message(x);
        }))
      , port(a.value.port)
    { }

    auto context() { return context_; }

    void start_accepting() {
      EM_ASM({
          var server = new window.WebSocket.Server({ port: $0 });
          Module.NBDL_DETAIL_JS_SET($1, server);
          var add_connection = Module.NBDL_DETAIL_JS_GET($2);
          var on_message     = Module.NBDL_DETAIL_JS_GET($3);
          server.on('connection', function connection(conn) {
            add_connection(conn);
            conn.on('message', function message(data) {
              on_message([conn, JSON.parse(data)]);
            });
          });
        },
        port,
        nbdl::js::get_handle(ws_val),
        nbdl::js::get_handle(add_connection_cb),
        nbdl::js::get_handle(on_message_cb)
      );
    }

    template <typename Message>
    void send_message(Message const& m) {
      // naively sends all messages to all connections
      auto& serialized_message = serializer_.serialize(m);
      for (auto& conn : connections) {
        EM_ASM({
            var conn    = Module.NBDL_DETAIL_JS_GET($0);
            var msg     = Module.NBDL_DETAIL_JS_GET($1);
            conn.send(JSON.stringify(msg))
          },
          nbdl::js::get_handle(conn),
          nbdl::js::get_handle(serialized_message)
        );
      }
    }

    void receive_message(nbdl::js::val js_val) {
      nbdl::js::val conn = js_val;
      nbdl::js::val msg  = std::move(js_val); 

      // destructuring [conn, msg]
      NBDL_JS_TRANSFORM(conn, function(val) {
        return val[0];
      });
      NBDL_JS_TRANSFORM(msg, function(val) {
        return val[1];
      });

      full_duplex::run_async_with_state(
        std::ref(*this),
        do_(
          full_duplex::promise_lift(std::move(msg)),
          deserializer_upstream<ContextTag>(),
          apply_read,
          full_duplex::catch_error([this, conn = std::move(conn)](auto&&) {
            std::cerr << '\n' << "Error occured: receive_message" << '\n';
            this->drop_connection(conn);
          })
        )
      );
    }

    void drop_connection(nbdl::js::val const& conn) {
      // actually disconnect
      EM_ASM({
          Module.NBDL_DETAIL_JS_GET($0).close();
        },
        nbdl::js::get_handle(conn)
      );

      // remove it from connections
      std::remove_if(connections.begin(), connections.end(),
        [&](nbdl::js::val& x) {
          return EM_ASM_INT({
              return Module.NBDL_DETAIL_JS_GET($0)
                  == Module.NBDL_DETAIL_JS_GET($1);
            },
            nbdl::js::get_handle(conn),
            nbdl::js::get_handle(x)
          );
        });
    }
  };
}

namespace nbdl {
  template <>
  struct consumer_init_impl<app::server> {
    template <typename Consumer>
    static void apply(Consumer& c) {
      c.start_accepting();
    }
  };

  template <typename Context>
  struct actor_type<app::server, Context> {
    using type = nbdl::app::server_impl<Context>;
  };

  template <>
  struct send_downstream_message_impl<app::server> {
    template <typename Consumer, typename Message>
    static void apply(Consumer& c, Message&& m) {
      c.send_message(std::forward<Message>(m));
    }
  };
}

#endif
