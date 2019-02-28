//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXT_ASIO_TCP_HPP
#define NBDL_EXT_ASIO_TCP_HPP

#include <full_duplex.hpp>
#include <optional>

#include <boost/asio.hpp>

namespace nbdl::asio_tcp
{
  using asio::ip::tcp;
  using full_duplex::do_;
  using full_duplex::endpoint;
  using full_duplex::make_error;
  using full_duplex::promise;

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

    tcp::socket socket_;
    tcp::endpoint endpoint;
    std::optional<tcp::acceptor> acceptor;
  };

  constexpr auto accept = promise([](auto& resolve, auto&&)
  {
    auto& state = resolve.get_state();
    state.acceptor = tcp::acceptor(
      state.socket().get_io_service()
    , state.endpoint
    );

    state.acceptor->async_accept(
      state.socket()
    , [&](auto error)
      {
        (not error) ? resolve(full_duplex::void_input) :
                      resolve(make_error(error));
      }
    );
  });

  // connect

  struct connect_state
  {
    connect_state(asio::io_service& io, unsigned short port)
      : socket_(io)
      , endpoint(tcp::v4(), port)
    { }

    tcp::socket& socket()
    { return socket_; }

    tcp::socket socket_;
    tcp::endpoint endpoint;
  };

  constexpr auto connect = promise([](auto& resolve, auto&&)
  {
    auto& state = resolve.get_state();
    state.socket().async_connect(
      state.endpoint
    , [&](auto error)
      {
        (not error) ? resolve(full_duplex::void_input) :
                      resolve(make_error(error));
      }
    );
  });

  // read message

  struct read_length_fn
  {
    template <typename Resolve, typename Input>
    auto operator()(Resolve& resolve, Input&&)
    {
      asio::async_read(
        self.state().socket()
      , asio::buffer(buffer, 4)
      , [&](auto error, size_t)
        {
          uint32_t length = buffer[0] << 24
                          | buffer[1] << 16
                          | buffer[2] << 8
                          | buffer[3];
          (not error) ? resolve(length) : resolve(make_error(error));
        }
      );
    }

    unsigned char buffer[4] = {};
  };

  struct read_body_fn
  {
    template <typename Resolve>
    auto operator()(Resolve& resolve, uint32_t length)
    {
      body.resize(length);
      asio::async_read(
        self.state().socket()
      , asio::buffer(body, length)
      , [&](auto error, size_t)
        { (not error) ? resolve(body) : resolve(make_error(error)); }
      );
    }

    std::string body = {};
  };

  constexpr auto read_message = do_(
    promise(read_length_fn{}),
    promise(read_body_fn{})
  );

  // write_message

  struct write_length_fn
  {
    template <typename Resolve, typename Message>
    auto operator()(Resolve& resolve, Message& m)
    {
      buffer[0] = static_cast<unsigned char>(m.size() >> 24);
      buffer[1] = static_cast<unsigned char>(m.size() >> 16);
      buffer[2] = static_cast<unsigned char>(m.size() >>  8);
      buffer[3] = static_cast<unsigned char>(m.size());

      asio::async_write(
        resolve.get_state().socket()
      , asio::buffer(buffer, 4)
      , [&](auto error, size_t)
        { (not error) ? resolve(m) : resolve(make_error(error)); }
      );
    }

    unsigned char buffer[4] = {};
  };

  constexpr auto write_message = do_(
    promise(write_length_fn{})
  , promise([](auto& resolve, auto& message)
    {
      asio::async_write(
        resolve.get_state().socket()
      , asio::buffer(message, message.size())
      , [&](auto error, size_t)
        { (not error) ? resolve(message) : resolve(make_error(error)); }
      );
    })
  );

  // endpoint "classes"

  constexpr auto acceptor   = endpoint(event::init = accept);
  constexpr auto connector  = endpoint(event::init = connect);

  constexpr auto message = endpoint(
    event::read_message  = read_message,
    event::write_message = write_message
  );
}

namespace nbdl
{
  // Just use the Beast Websockets integration
  //
  // TODO implement Producer/Consumer
}

#endif
