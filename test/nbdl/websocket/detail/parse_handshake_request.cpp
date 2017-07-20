//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/hana/functional/always.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket/detail/parse_handshake_request.hpp>
#include <nbdl/tap.hpp>
#include <nbdl_test/asio_helpers.hpp>
#include <string>

namespace hana = boost::hana;

namespace
{
  using nbdl::websocket::detail::handshake_info_t;
  using nbdl::websocket::event::bad_request_t;

  inline handshake_info_t run_test(std::string const& request)
  {
    handshake_info_t handshake_info{};

    // TODO make a socket holder
    asio::io_service io;
    auto server_socket = hana::always(asio::ip::tcp::socket(io));
    auto client_socket = hana::always(asio::ip::tcp::socket(io));

    nbdl::run_async(hana::make_tuple(
      std::ref(server_socket)
    , nbdl_test::accept()
    , nbdl::websocket::detail::parse_handshake_request()
    , nbdl::tap([&](auto&&, handshake_info_t h) { handshake_info = h; })
    , nbdl::catch_([](asio::error_code) { CHECK(false); })
    , nbdl::catch_([](hana::basic_type<bad_request_t>) { CHECK(false); })
    ));

    nbdl::run_async(hana::make_tuple(
      std::ref(client_socket)
    , nbdl_test::connect()
    , nbdl_test::send_data(request)
    , nbdl::catch_([](auto) { CHECK(false); })
    ));

    io.run();

    return handshake_info;
  }
}

TEST_CASE("Request parser should populate pertinent header values.", "[websocket]") 
{
  std::string request =
    "GET / HTTP/1.1"
    "\r\n"
    "Host: example.com:8000"
    "\r\n"
    "Origin: http://myorigin.com"
    "\r\n"
    "Upgrade: websocket"
    "\r\n"
    "Connection: Upgrade"
    "\r\n"
    "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ=="
    "\r\n"
    "Sec-WebSocket-Version: 13"
    "\r\n"
    "Cookies: nbdl-auth:adslkjfasdf; blah=asdfasd;"
    "\r\n"
    "\r\n"
    ;

  auto handshake_info = run_test(request);

  CHECK(handshake_info.websocket_key == "dGhlIHNhbXBsZSBub25jZQ==");
  CHECK(handshake_info.cookies == "nbdl-auth:adslkjfasdf; blah=asdfasd;");
}
