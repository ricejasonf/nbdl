//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVER_CONNECTION_HPP
#define NBDL_SERVER_CONNECTION_HPP

// The Consumer that manages messaging to socket clients

#include <full_duplex.hpp>
#include <full_duplex/future.hpp>
#include <functional>
#include <nbdl.hpp>
#include <nbdl/app/context_endpoint.hpp>
#include <nbdl/binder/jsoncpp.hpp>
#include <nbdl/ext/beast_ws.hpp>
#include <queue>
#include <string>
#include <vector>

namespace nbdl::app {
  namespace beast_ws = nbdl::ext::beast_ws;
  namespace hana = boost::hana;
  using full_duplex::do_;
  using full_duplex::map;
  using full_duplex::promise;
  using full_duplex::tap;
  using full_duplex::void_input;

  constexpr auto authenticate = promise([](auto& resolve, auto&& x) {
    // we should already have a token from a cookie or something
    // check against the db or whatever
    // must resolve self
    resolve(std::forward<decltype(x)>(x));
  });

#if 0 // I dont' think these are used
  constexpr auto serialize_message = [] {
    return do_(
      // TODO map to downstream variant
      tap([](auto&& foo) { foo.foo(); }),
      map(nbdl::binder::jsoncpp::to_string),
      map(full_duplex::future<std::string>())
    );
  };

  constexpr auto deserialize_message = [] {
    return promise([](auto& resolve,
                      std::string const& msg_buf) {
      using full_duplex::make_error;
      api::upstream_variant var;

      try {
        nbdl::binder::jsoncpp::from_string(msg_buf, var);
      } catch(...) {
        resolve(make_error("JSONCPP Parse Error"));
      }

      nbdl::match(var, hana::overload_linearly(
        [&resolve](nbdl::unresolved) {
          // this should not happen unless we get
          // garbage input or something
          resolve(make_error("Unknown Error"));
        },
        [&resolve](system_message sys_msg) {
          // something bad happened?
          resolve(make_error(std::move(sys_msg)));
        },
        [&resolve](auto&& msg) {
          resolve(std::forward<decltype(msg)>(msg));
        }));
    });
  };
#endif

  constexpr auto register_conn = [](auto& self) {
    return tap([&](auto&&) {
      self.state().register_connection(self);
    });
  };

  constexpr auto unregister_conn = [](auto& self) {
    return tap([&](auto&&) {
      self.state().unregister_connection(self);
    });
  };

  template <typename ContextTag, typename State>
  auto connection_open(State state) {
    using full_duplex::do_;
    using full_duplex::endpoint;
    using full_duplex::endpoint_compose;
    using full_duplex::endpoint_open;
    using full_duplex::map;
    namespace event = full_duplex::event;

    // It's fine to just unwrap the smart pointer here.
    // `unregister_conn` removes itself from the list
    // before the resource is destroyed
    //
    // This is slightly better than copying the shared_ptr
    // just to find it in the container
    // (it would have to via shared_from_this)
    return full_duplex::endpoint_open(
      std::move(state),
      std::queue<std::string>{},
      endpoint_compose(
        beast_ws::message_endpoint,
        endpoint(
          event::init           = register_conn,
          event::read_message   = do_(deserializer_upstream<ContextTag>(),
                                      apply_read),
          event::error          = log_error,
          event::terminate      = unregister_conn
        )
      )
    ).get();
  }
}

#endif
