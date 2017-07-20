//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_PARSE_HANDSHAKE_RESPONSE_HPP
#define NBDL_WEBSOCKET_DETAIL_PARSE_HANDSHAKE_RESPONSE_HPP

#include <nbdl/websocket/detail/expect_equal.hpp>

#include <string_view>

namespace nbdl::websocket::detail
{
  using std::string_view;

  constexpr auto parse_handshake_response = []()
  {
    // TODO Server needs to set cookies
    // which could specify a session token
    constexpr string_view expected_response = string_view(
      "HTTP/1.1 101 Switching Protocols"
      "\r\n"
      "Upgrade: websocket"
      "\r\n"
      "Connection: Upgrade"
      "\r\n"
      "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
      "\r\n"
      "\r\n"
    );
    return expect_equal(expected_response);
  };
}

#endif
