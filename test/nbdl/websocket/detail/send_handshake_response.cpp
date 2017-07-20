//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/hana/functional/always.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <catch.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket/detail/parse_handshake_request.hpp>
#include <nbdl/websocket/detail/send_handshake_response.hpp>
#include <nbdl/tap.hpp>
#include <nbdl_test/asio_helpers.hpp>
#include <string>
#include <string_view>

namespace hana = boost::hana;

namespace
{
  using nbdl::websocket::detail::handshake_info_t;

  inline void run_test(handshake_info_t const& handshake_info, std::string& response_string)
  {
    asio::io_service io;
    auto server_socket = hana::always(asio::ip::tcp::socket(io));
    auto client_socket = hana::always(asio::ip::tcp::socket(io));

    nbdl::run_async(hana::make_tuple(
      std::ref(server_socket)
    , nbdl_test::accept()
    , nbdl::promise([&](auto& resolve, auto& socket) { resolve(socket, handshake_info); })
    , nbdl::websocket::detail::send_handshake_response()
    , nbdl::catch_([](asio::error_code) { CHECK(false); })
    ));

    nbdl::run_async(hana::make_tuple(
      std::ref(client_socket)
    , nbdl_test::connect()
    , nbdl_test::receive_data(response_string)
    , nbdl::catch_([](auto) { CHECK(false); })
    ));

    io.run();
  }
}

TEST_CASE("Generated Response should contain the proper Sec-WebSocket-Accept string.", "[websocket]") 
{
  using nbdl::websocket::detail::handshake_info_t;

	constexpr std::string_view expected(
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

  handshake_info_t handshake_info{};
  handshake_info.websocket_key = std::string("dGhlIHNhbXBsZSBub25jZQ==");

  std::string response(expected.size(), 0);

	run_test(handshake_info, response);
	CHECK(response == expected);
}
