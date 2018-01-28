//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_API_CLIENT_ENDPOINT_HPP
#define NBDL_WEBSOCKET_API_CLIENT_ENDPOINT_HPP

#include <nbdl/endpoint_open.hpp>
#include <nbdl/websocket_api/detail/endpoint_impl.hpp>
#include <nbdl/fwd/websocket_api/client_endpoint.hpp>

#include <utility>

namespace nbdl
{
  template <typename Derived>
  struct endpoint_open_impl<nbdl::websocket_api::client_endpoint_base<Derived>>
  {
    template <typename Endpoint, typename Queue, typename Handler>
    static auto apply(Endpoint&& endpoint, Queue&& queue, Handler&& handler)
    {
      using DecayedQueue   = std::decay_t<Queue>;
      using DecayedHandler = std::decay_t<Handler>;

      return nbdl::websocket_api::detail::endpoint_impl<DecayedQueue, DecayedHandler, Derived>(
        std::forward<Endpoint>(endpoint)
      , std::forward<Queue>(queue)
      , std::forward<Handler>(handler)
      );
    }
  };
}

#endif
