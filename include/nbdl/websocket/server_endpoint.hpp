//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_SERVER_ENDPOINT_HPP
#define NBDL_WEBSOCKET_SERVER_ENDPOINT_HPP

#include <nbdl/catch.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/endpoint_open.hpp>
#include <nbdl/fwd/websocket/server_endpoint.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/websocket/detail/endpoint_impl.hpp>
#include <nbdl/websocket/detail/parse_handshake_request.hpp>
#include <nbdl/websocket/detail/send_handshake_response.hpp>

#include <asio.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace websocket::detail
  {
    using tcp = asio::ip::tcp;
    constexpr auto server_endpoint_init = [](auto& self, tcp::socket& socket, auto& handler)
    {
      nbdl::run_async(
        hana::make_tuple(
          websocket::detail::parse_handshake_request()
        , websocket::detail::send_handshake_response()
        , [&](tcp::socket&, auto const& auth_token)
          {
            handler[endpoint_event::ready](self, auth_token);
            self._start_reading();
          }
        , nbdl::catch_([&](auto&& error)
          {
            handler[hana::typeid_(error)](self, std::forward<decltype(error)>(error));
          })
        )
      , socket
      );
    };
  }

  template <typename SendMessageImpl, typename Derived>
  struct endpoint_open_impl<nbdl::websocket::server_endpoint_impl<SendMessageImpl, Derived>>
  {
    using tcp = asio::ip::tcp;

    template <typename Endpoint, typename Queue, typename Handler>
    static auto apply(Endpoint&& endpoint, Queue&& queue, Handler&& handler)
    {
      using DecayedQueue   = std::decay_t<Queue>;
      using DecayedHandler = std::decay_t<Handler>;

      return websocket::detail::endpoint_impl<DecayedQueue
                                            , DecayedHandler
                                            , SendMessageImpl
                                            , Derived>(
        std::forward<Endpoint>(endpoint).socket
      , std::forward<Queue>(queue)
      , std::forward<Handler>(handler)
      , websocket::detail::server_endpoint_init
      );
    }
  };
}

#endif
