//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBSOCKET_CLIENT_ENDPOINT_HPP
#define NBDL_FWD_WEBSOCKET_CLIENT_ENDPOINT_HPP

#include <nbdl/auth_token.hpp>
#include <nbdl/websocket/detail/endpoint_impl.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <string>

namespace nbdl::websocket
{
  struct origin_t
  {
    std::string value;
  };

  template <typename SendMessageImpl, typename Derived = void>
  struct client_endpoint_impl
  {
    boost::asio::ip::tcp::socket& socket;
    auth_token_t auth_token;
    origin_t origin;
  };

  using client_endpoint         = client_endpoint_impl<decltype(detail::send_binary)>;
  using client_endpoint_text    = client_endpoint_impl<decltype(detail::send_text)>;

  template <typename Derived>
  using client_endpoint_base         = client_endpoint_impl<decltype(detail::send_binary), Derived>;
  template <typename Derived>
  using client_endpoint_text_base    = client_endpoint_impl<decltype(detail::send_text), Derived>;
}

#endif
