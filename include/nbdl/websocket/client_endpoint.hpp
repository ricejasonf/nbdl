//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_CLIENT_ENDPOINT_HPP
#define NBDL_WEBSOCKET_CLIENT_ENDPOINT_HPP

#include <nbdl/catch.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/endpoint_open.hpp>
#include <nbdl/fwd/websocket/client_endpoint.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket/detail/endpoint_impl.hpp>
#include <nbdl/websocket/detail/parse_handshake_response.hpp>
#include <nbdl/websocket/detail/send_handshake_request.hpp>

#include <asio.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename SendMessageImpl>
  struct endpoint_open_impl<nbdl::websocket::client_endpoint_impl<SendMessageImpl>>
  {
    template <typename Endpoint, typename Queue, typename Handler>
    static auto apply(Endpoint&& endpoint, Queue&& queue, Handler&& handler)
    {
      using DecayedQueue   = std::decay_t<Queue>;
      using DecayedHandler = std::decay_t<Handler>;

      auto [socket, auth_token, origin] = std::forward<Endpoint>(endpoint);

      return websocket::detail::endpoint_impl<DecayedQueue
                                            , DecayedHandler
                                            , SendMessageImpl>(
        std::move(socket)
      , std::forward<Queue>(queue)
      , std::forward<Handler>(handler)
      , [ origin = origin
        , auth_token = auth_token
        ](auto& self, auto& socket, auto& handler)
        {
          nbdl::run_async(
            hana::make_tuple(
              websocket::detail::send_handshake_request(origin, auth_token)
            , websocket::detail::parse_handshake_response()
            , [&](auto&&)
              {
                self._start_reading();
                handler[endpoint_event::ready](self);
              }
            , nbdl::catch_([&](auto&& error)
              {
                handler[hana::typeid_(error)](self, std::forward<decltype(error)>(error));
              })
            )
          , socket
          );
        }
      );
    }
  };
}

#endif
