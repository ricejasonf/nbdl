//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_SEND_HANDSHAKE_RESPONSE_HPP
#define NBDL_WEBSOCKET_DETAIL_SEND_HANDSHAKE_RESPONSE_HPP

#include <nbdl/detail/beast_sha1.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/util/base64_encode.hpp>
#include <nbdl/websocket/detail/get_auth_token.hpp>
#include <nbdl/websocket/detail/parse_handshake_request.hpp>

#include <array>
#include <asio.hpp>
#include <string_view>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  using namespace std::string_view_literals;
  using std::string_view;

  inline std::string generate_accept_token(std::string const& key)
  {
    namespace sha1 = nbdl::detail::beast_sha1;
    using nbdl::util::base64_encode;

    constexpr string_view guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"sv;
    std::array<unsigned char, 20> digest{};
    sha1::sha1_context ctx{};

    sha1::init(ctx);
    sha1::update(ctx, key.data(), key.length());
    sha1::update(ctx, guid.data(), guid.size());
    sha1::finish(ctx, digest.data());

    return base64_encode(digest);
  }

  struct send_handshake_response_fn
  {
    template <typename Resolver>
    auto operator()(Resolver& resolver, tcp::socket& socket, handshake_info_t const& handshake_info)
    {
      auto const& [websocket_key, cookies] = handshake_info;
      accept_token = generate_accept_token(websocket_key);

      const Buffers buffers{{
        asio::buffer(response_start)
      , asio::buffer(accept_token)
      , asio::buffer(response_end)
      }};

      asio::async_write(socket, buffers, [&, cookies = cookies](auto error_code, std::size_t)
      {
        if (error_code)
        {
          resolver.reject(error_code);
        }
        else
        {
          resolver.resolve(socket, detail::get_auth_token(cookies));
        }
      });
    }

  private:

    using Buffers = std::array<asio::const_buffer, 3>;

    static constexpr string_view response_start = string_view(
      "HTTP/1.1 101 Switching Protocols"
      "\r\n"
      "Upgrade: websocket"
      "\r\n"
      "Connection: Upgrade"
      "\r\n"
      "Sec-WebSocket-Accept: "
    );
    static constexpr string_view response_end = string_view("\r\n\r\n");

    std::string accept_token{};
  };

  constexpr auto send_handshake_response = [] { return nbdl::promise(send_handshake_response_fn{}); };
}

#endif
