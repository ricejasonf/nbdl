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

#include <full_duplex.hpp>
#include <nbdl.hpp>
#include <nbdl/ext/beast_ws.hpp>
#include <string>
#include <utility>
#include <tuple>

namespace nbdl::app::client_connection_detail {
  namespace beast_ws = ext::beast_ws;
  using full_duplex::promise;

  // returns [protocol, domain, port]
  constexpr
  std::tuple<std::string_view, std::string_view, std::string_view>
  split_address(std::string_view address) {
    auto pos = address.find("://");
    std::string_view protocol{};
    std::string_view port{};

    if (pos != std::string_view::npos) {
      protocol = std::string_view(address).substr(pos);
      address.remove_prefix(pos + 3);
    }

    pos = address.find(":");
    if (pos != std::string_view::npos) {
      port = address.substr(pos + 1);
      address = address.substr(0, pos);
    }

    return {protocol, address, port};
  }

  // just a way to keep stuff alive
  template <typename ClientImplRef>
  struct state {
    ClientImplRef client_impl_ref;
    beast_ws::stream_t stream_;

    state(ClientImplRef c)
      : client_impl_ref(c)
      , stream_(c.get().io)
    { }

    auto& io() const {
      return client_impl_ref.get().io;
    }

    std::string const& address() const {
      return client_impl_ref.get().address;
    }

    auto context() const {
      return client_impl_ref.get().context;
    }

    auto& stream()   { return stream_; }
    auto& socket()   { return stream_.next_layer(); }
  };

  constexpr auto init = promise([](auto& resolve, auto&&) {
    auto& io = resolve.get_state().io();
    auto [protocol, host, port] = split_address(resolve.get_state().address());

    // TODO move this promise_any thing into full_duplex
    //      It just makes a nested async action finish the parent promise
    //      which is very useful
    full_duplex::run_async_with_state(
      std::ref(resolve.get_state()),
      beast_ws::connect(io, std::string(host), std::string(port)),
      full_duplex::promise_any([&resolve](auto& resolve_jr, auto&& result) {
        // let the nested promise be destroyed first which destroys this lambda
        auto& resolve_ = resolve;
        auto result_ = std::forward<decltype(result)>(result);
        resolve_jr(full_duplex::terminate{});
        resolve_(std::move(result_));
      })
    );
  });

  constexpr auto read_message  = beast_ws::read_message;
  constexpr auto write_message = beast_ws::write_message;
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
      std::queue<std::string>{},
      endpoint_compose(
        client_connection_endpoint,
        std::forward<Endpoint>(endpoint)
      )
    );
  }
}

#endif // EMSCRIPTEN
#endif
