//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_ENDPOINT_HPP
#define NBDL_WEBSOCKET_ENDPOINT_HPP

#include <nbdl/endpoint_open.hpp>
#include <nbdl/websocket/detail/endpoint_impl.hpp>

#include <boost/asio.hpp>

namespace nbdl::websocket
{
  template <typename SendMessageImpl>
  struct endpoint
  {
    boost::asio::ip::tcp::socket socket;
  };

  using endpoint_text   = endpoint<decltype(detail::send_text)>;
  using endpoint_binary = endpoint<decltype(detail::send_binary)>;
}

namespace nbdl
{
  template <typename SendMessageImpl>
  struct endpoint_open_impl<nbdl::websocket::endpoint<SendMessageImpl>>
  {
    template <typename Endpoint, typename Queue, typename Handler>
    static auto apply(Endpoint&& endpoint, Queue&& queue, Handler&& handler)
    {
      return nbdl::websocket::detail::endpoint_impl(
        std::forward<Endpoint>(endpoint).socket
      , std::forward<Queue>(queue)
      , std::forward<Handler>(handler)
      , SendMessageImpl{}
      );
    }
  };
}

#endif
