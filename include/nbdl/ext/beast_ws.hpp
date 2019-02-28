//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXT_BEAST_WS_HPP
#define NBDL_EXT_BEAST_WS_HPP

#include <nbdl/ext/asio_tcp.hpp>

#include <boost/beast.hpp>
#include <full_duplex.hpp>
#include <optional>
#include <string>

namespace nbdl::beast_ws
{
  using asio::ip::tcp;

  // accept

  struct accept_state
  {
    accept_state(asio::io_service& io, unsigned short port)
        : ws(io)
        , endpoint(tcp::v4(), port)
        , acceptor()
    { }

    tcp::socket& socket()
    { return ws.next_layer(); }

    boost::beast::websocket::stream<tcp::socket> ws;
    tcp::endpoint endpoint;
    std::optional<tcp::acceptor> acceptor;
  };

  struct connect_state
  {
    connect_state(asio::io_service& io, 
                  std::string host,
                  std::string target
                  unsigned short port)
      : ws(io)
      , host(std::move(host))
      , target(std::move(target))
      , endpoint(tcp::v4(), port)
    { }

    tcp::socket& socket()
    { return ws.next_layer(); }

    std::string host;
    std::string target;
    boost::beast::websocket::stream<tcp::socket> ws;
    tcp::endpoint endpoint;
  };
}

namespace nbdl::beast_ws::detail
{
  using full_duplex::do_;
  using full_duplex::make_error;
  using full_duplex::promise;


  constexpr auto accept = promise([](auto& resolve, auto&&)
  {
    // This does the websocket handshake
    resolve.get_state().ws.async_accept([&](auto error)
    {
      (not error) ? resolve(full_duplex::void_input) :
                    resolve(make_error(error));
    });
  });

  // connect

  constexpr auto send_handshake = promise([](auto& resolve, auto&&)
  {
    auto& state = resolve.get_state();
    state.ws.async_handshake(
      state.host
    , state.target
    , [&](auto error)
      {
        (not error) ? resolve(full_duplex::void_input) :
                      resolve(make_error(error));
      });
  });

  // read_message

  struct read_message_fn
  {
    template <typename Resolve, typename Input>
    void operator()(Resolve& resolve, Input&&)
    {
      resolve.get_state().ws.async_read(buffer, [&](auto error, size_t)
      {
        auto mut_buf = buffer.data();
        std::string_view buffer_view(static_cast<char const*>(mut_buf.data()), mut_buf.size());
        body.resize(buffer_view.size());
        std::copy(buffer_view.begin(), buffer_view.end(), body.begin()); // :(
        buffer.consume(buffer.size());
        (not error) ? resolve(body) : resolve(make_error(error));
      });
    }

    std::string body = {};
    boost::beast::flat_buffer buffer = {};
  };

  constexpr auto read_message = promise(read_message_fn{});

  // write_message

  constexpr auto write_message = promise([](auto& resolve, auto& message)
  {
    resolve.get_state().ws.async_write(
      asio::buffer(message, message.size()),
      [&](auto error, size_t)
      { (not error) ? resolve(message) : resolve(make_error(error)); }
    );
  });

  // server_endpoint

  template <typename Queue, typename ...Events>
  auto server_endpoint_open(accept_state state, Queue&& queue,
                            Events&& ...events)
  {
    return full_duplex::endpoint_open(
      std::move(state)
    , std::forward<Queue>(queue)
    , full_duplex::endpoint_compose(
        endpoint(event::init = asio_tcp::accept)
      , endpoint(
          event::init           = accept
        , event::read_message   = read_message
        , event::write_message  = write_message
        )
      , endpoint(std::forward<Events>(events))
      )
    )
  };

  // client_endpoint

  template <typename Queue, typename ...Events>
  auto client_endpoint_open(connect_state state, Queue&& queue,
                            Events&& ...events)
  {
    return full_duplex::endpoint_open(
      std::move(state)
    , std::forward<Queue>(queue)
    , full_duplex::endpoint_compose(
        endpoint(event::init = asio_tcp::connect)
      , endpoint(
          event::init           = send_handshake
        , event::read_message   = read_message
        , event::write_message  = write_message
        )
      , endpoint(std::forward<Events>(events)...)
      )
    );
  };
}

namespace nbdl
{
  // Producer

  // Consumer
}

#endif
