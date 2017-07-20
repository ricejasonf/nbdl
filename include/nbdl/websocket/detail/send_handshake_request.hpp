//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_SEND_HANDSHAKE_REQUEST_HPP
#define NBDL_WEBSOCKET_DETAIL_SEND_HANDSHAKE_REQUEST_HPP

#include <nbdl/auth_token.hpp>
#include <nbdl/detail/make_array.hpp>
#include <nbdl/fwd/websocket/client_endpoint.hpp>
#include <nbdl/promise.hpp>

#include <asio.hpp>
#include <string_view>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  using namespace std::string_view_literals;
  using std::string_view;

  struct send_handshake_request_fn
  {
    origin_t origin;
    auth_token_t auth_token;

    template <typename Resolver>
    auto operator()(Resolver& resolver, tcp::socket& socket)
    {
      auto request_buffers = ::nbdl::detail::make_array<>(
        asio::buffer(string_view(
          "GET / HTTP/1.1"
          "\r\n"
          "Host: nbdl"
          "\r\n"
          "Origin:"
        ))
      , asio::const_buffer(asio::buffer(origin.value))
      , asio::buffer(string_view(
          "\r\n"
          "Upgrade: websocket"
          "\r\n"
          "Connection: Upgrade"
          "\r\n"
          "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ=="
          "\r\n"
          "Sec-WebSocket-Version: 13"
          "\r\n"
          "Cookies: nbdl_auth="
        ))
      , asio::const_buffer(asio::buffer(auth_token.value))
      , asio::buffer(string_view(
          ";"
          "\r\n"
          "\r\n"
        ))
      );

      asio::async_write(socket, std::move(request_buffers), [&](std::error_code error, std::size_t)
      {
        if (error)
        {
          resolver.reject(error);
        }
        else
        {
          resolver.resolve(socket);
        }
      });
    }

  private:

  };

  constexpr auto send_handshake_request = [](auto&& origin, auto&& session_token)
  {
    return nbdl::promise(send_handshake_request_fn{
      std::forward<decltype(origin)>(origin)
    , std::forward<decltype(session_token)>(session_token)
    });
  };
}

#endif
