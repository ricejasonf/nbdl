//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_CLIENT_CONNECTION_HPP
#define NBDL_APP_CLIENT_CONNECTION_HPP

#ifdef EMSCRIPTEN
#include <nbdl/app/client_connection_js.hpp>
#else // NOT EMSCRIPTEN

#include <nbdl.hpp>
#include <full_duplex.hpp>

#include <string>

namespace nbdl::app::client_connection_detail {
  using full_duplex::promise;

  // just a way to keep stuff alive
  template <typename ClientImplRef>
  struct state {
    ClientImplRef client_impl_ref;

    std::string const& address() const {
      return client_impl_ref.get().address;
    }

    auto context() const {
      return client_impl_ref.get().context;
    }
  };

  constexpr auto init = promise([](auto& resolve, auto&&) {
    // TODO initialize client connection
  });

  constexpr auto read_message = promise([](auto& resolve, auto&&) {
    // TODO beast async read_message
  });

  constexpr auto write_message = promise([](auto& resolve, nbdl::js::val const& msg) {
    // TODO beast async write message
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

#endif // EMSCRIPTEN
#endif
