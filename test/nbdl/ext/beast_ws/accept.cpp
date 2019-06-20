//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/ext/beast_ws.hpp>

#include <boost/asio.hpp>
#include <catch.hpp>
#include <full_duplex.hpp>
#include <string>
#include <utility>

namespace asio     = boost::asio;
namespace beast    = boost::beast;
namespace beast_ws = nbdl::ext::beast_ws;
namespace hana     = boost::hana;
using tcp          = boost::asio::ip::tcp;

namespace {
  using stream_t = beast::websocket::stream<tcp::socket>;

  // TODO these should satisfy documented constraints
  struct accept_t {
    tcp::acceptor acceptor_;
    stream_t stream_;

    accept_t(tcp::acceptor acceptor)
      : acceptor_(std::move(acceptor))
      , stream_(acceptor_.get_executor())
    { }

    auto& acceptor() { return acceptor_; }
    auto& stream()   { return stream_; }
    auto& socket()   { return stream_.next_layer(); }
  };

  struct connect_t {
    stream_t stream_;

    auto& stream()   { return stream_; }
    auto& socket()   { return stream_.next_layer(); }
  };
}

TEST_CASE("Accept a websocket connection.", "[ext][beast_ws]")
{
  constexpr auto catch_error = full_duplex::catch_error;
  using error_code = boost::system::error_code;
  asio::io_service io;
  bool did_accept = false;
  tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));

  // accept

  full_duplex::run_async_with_state(
    accept_t{
      tcp::acceptor(io, tcp::endpoint(tcp::v4(), 60080))
    },
    beast_ws::accept,
    full_duplex::tap([&](auto& /* stream */) {
      // TODO check session token
      did_accept = true;
    }),
    catch_error([](error_code error) { CHECK(error.message() == ""); })
  );

  // connect

  full_duplex::run_async_with_state(
    connect_t{stream_t{io}},
    beast_ws::connect(io, "localhost", "60080"),
    catch_error([](error_code error) { CHECK(error.message() == ""); })
  );
    
  // event loop

  io.run();

  // check stuff
  REQUIRE(did_accept);
}
