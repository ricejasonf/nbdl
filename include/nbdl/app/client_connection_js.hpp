// // Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_CLIENT_CONNECTION_JS_HPP
#define NBDL_APP_CLIENT_CONNECTION_JS_HPP

#ifndef EMSCRIPTEN
#error "client_connection_js.hpp requires an emscripten environment."
#endif

#include <full_duplex.hpp>
#include <functional>
#include <nbdl.hpp>
#include <nbdl/app/serializer.hpp>
#include <nbdl/js.hpp>

namespace nbdl::app::client_connection_detail {
  using full_duplex::promise;
  using nbdl::js::get_handle;
  using nbdl::js::callback;
  using nbdl::js::callback_t;

  // just a way to keep stuff alive
  template <typename ClientImplRef>
  struct state {
    ClientImplRef client_impl_ref;
    callback_t ready_cb   = callback([](auto&&) { });
    callback_t message_cb = callback([](auto&&) { });
    callback_t error_cb   = callback([](auto&&) { });
    nbdl::js::val ws_val  = {};

    std::string const& address() const {
      return client_impl_ref.get().address;
    }

    auto context() const {
      return client_impl_ref.get().context;
    }
  };

  constexpr auto init = promise([](auto& resolve, auto&&) {
    std::string const& address = resolve.get_state().address();
    resolve.get_state().ready_cb = callback(std::ref(resolve));
    resolve.get_state().error_cb = callback([&](nbdl::js::val x) {
      resolve(full_duplex::make_error(std::move(x)));
    });
    EM_ASM_({
          var ready_cb = Module.NBDL_DETAIL_JS_GET($3); 
          var error_cb = Module.NBDL_DETAIL_JS_GET($4); 
          var ws_val = ({
            ws: new window.WebSocket(UTF8ToString($0, $1)),
            error: null
          });

          Module.NBDL_DETAIL_JS_SET($2, ws_val);
          var s = ws_val.ws;
          s.binaryType = 'arraybuffer';

          function close_out() {
            ready_cb = function() { };
            error_cb = function() { };
          };

          s.addEventListener('open', function(event) {
            ready_cb(event);
            close_out();
          });

          s.addEventListener('error', function(event) {
            ws_val.error = event;
            error_cb(event);
            close_out();
          });

          s.addEventListener('close', function(event) {
            ws_val.error = "close";
            error_cb(event);
            close_out();
          });
      },
      address.data(),
      address.size(),
      get_handle(resolve.get_state().ws_val),
      get_handle(resolve.get_state().ready_cb),
      get_handle(resolve.get_state().error_cb)
    );
  });

  constexpr auto read_message = promise([](auto& resolve, auto&&) {
    std::string const& address = resolve.get_state().address();
    resolve.get_state().message_cb = callback([&](nbdl::js::val value) {
      resolve(std::move(value));
    });
    resolve.get_state().error_cb   = callback([&](nbdl::js::val x) {
      resolve(full_duplex::make_error(std::move(x)));
    });

    // NOTE: The callbacks aren't guaranteed to have the same
    //       address so we reset the event listeners each time.

    EM_ASM_({
          var ws_val     = Module.NBDL_DETAIL_JS_GET($2); 
          var message_cb = Module.NBDL_DETAIL_JS_GET($3); 
          var error_cb   = Module.NBDL_DETAIL_JS_GET($4); 

          var s = ws_val.ws;
          s.binaryType = 'arraybuffer';

          var error_fn = function(event) {
            error_cb(event);
            clean_up_listeners();
          };

          var close_fn = function(event) {
            error_cb(event);
            clean_up_listeners();
          };

          var message_fn = function(event) {
            message_cb(JSON.parse(event.data));
            clean_up_listeners();
          };

          function clean_up_listeners() {
            s.removeEventListener('error', error_fn);
            s.removeEventListener('close', close_fn);
            s.removeEventListener('message', message_fn);
            // ensure these never get called again
            message_cb = function() { };
            error_cb   = function() { };
          };

          s.addEventListener('error', error_fn);
          s.addEventListener('close', close_fn);
          s.addEventListener('message', message_fn);
      },
      address.data(),
      address.size(),
      get_handle(resolve.get_state().ws_val),
      get_handle(resolve.get_state().message_cb),
      get_handle(resolve.get_state().error_cb)
    );
  });

  constexpr auto write_message = promise([](auto& resolve, nbdl::js::val const& msg) {
    EM_ASM_({
        var ws = Module.NBDL_DETAIL_JS_GET($0).ws;
        var msg = JSON.stringify(Module.NBDL_DETAIL_JS_GET($1));
        ws.send(msg);
      },
      nbdl::js::detail::get_handle(resolve.get_state().ws_val),
      nbdl::js::detail::get_handle(msg)
    );
    resolve(full_duplex::void_input);
  });
}

namespace nbdl::app {
  namespace event = full_duplex::event;
  using full_duplex::do_;
  using full_duplex::endpoint;
  using full_duplex::endpoint_compose;

  constexpr auto client_connection_endpoint = endpoint(
    event::init           = client_connection_detail::init,
    event::read_message   = client_connection_detail::read_message,
    event::write_message  = client_connection_detail::write_message
  );

  template <typename ClientImplRef, typename Endpoint>
  auto client_endpoint_open(ClientImplRef impl, Endpoint&& endpoint) {
    using state = nbdl::app::client_connection_detail::state<ClientImplRef>;
    return full_duplex::endpoint_open(
      state{impl},
      std::queue<nbdl::js::val>{},
      endpoint_compose(
        client_connection_endpoint,
        std::forward<Endpoint>(endpoint)
      )
    );
  }
}

#endif
