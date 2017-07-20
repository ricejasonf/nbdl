//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBSOCKET_SERVER_ENDPOINT_HPP
#define NBDL_FWD_WEBSOCKET_SERVER_ENDPOINT_HPP

#include <nbdl/websocket/detail/endpoint_impl.hpp>

#include <asio/ip/tcp.hpp>
#include <string>

namespace nbdl::websocket
{
  template <typename SendMessageImpl>
  struct server_endpoint_impl
  {
    asio::ip::tcp::socket& socket;
  };

  using server_endpoint         = server_endpoint_impl<decltype(detail::send_binary)>;
  using server_endpoint_text    = server_endpoint_impl<decltype(detail::send_text)>;
}

#endif
