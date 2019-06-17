//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXT_BEAST_WS_HPP
#define NBDL_EXT_BEAST_WS_HPP

#include <boost/beast.hpp>
#include <full_duplex.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace nbdl::beast_ws
{
  namespace asio  = boost::asio;
  namespace beast = boost::beast;
  namespace event = full_duplex::event;
  using full_duplex::make_error;
  using full_duplex::promise;
  using full_duplex::promise_lift;

  using asio::ip::tcp;

  // accept -/ Promise that resolves a
  // beast::websocket<tcp::stream>
  // and (eventually) http request info
  // or an error
  //
  constexpr auto accept = full_duplex::do_(
    promise([](auto& resolve, auto&&) {
      // TODO a concept for state would be nice
      auto& acceptor = resolve.get_state().acceptor();
      auto& socket   = resolve.get_state().socket();
      acceptor.async_accept(socket, [&resolve, &socket](auto error) {
        // it's normal to move the socket
        // and recycle the original
        (not error) ? resolve(std::move(socket)) :
                      resolve(make_error(error));
      });
    }),
    promise([](auto& resolve, tcp::socket socket) {
      beast::websocket::stream<tcp::socket>
      stream{std::move(tcp::socket)};

      stream.async_accept([&resolve, &stream](auto error) {
        (not error) ? resolve(std::move(stream)) :
                      resolve(make_error(error));
      });
      // TODO get session cookie somehow
    })
  );

  // connect - currently used for integration testing
  constexpr auto connect = [](std::string host) {
    return full_duplex::do_(
      promise([&](auto& resolve, auto&&) {
          auto& socket = resolve.get_state().socket();
          socket.async_connect(state.endpoint, [&](auto error) {
              (not error) ? resolve(self) : resolve(make_error(error));
          });
      }),
      promise([host = std::move(host)](auto& resolve, auto&&) {
          auto& stream = resolve.get_state().stream();
          state.ws.async_handshake(boost::string_view(host),
                                   boost::string_view("/"),
                                   [&](auto error) {
            (not error) ? resolve(self) : resolve(make_error(error));
          });
      })
    );
  };

  // read_message

  struct read_message_fn
  {
    template <typename Resolve, typename Input>
    void operator()(Resolve& resolve, Input&&)
    {
      resolve.get_state().socket.async_read(buffer, [&](auto error, size_t)
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
    beast::flat_buffer buffer = {};
  };

  constexpr auto read_message = promise(read_message_fn{});

  // write_message

  constexpr auto write_message = promise([](auto& resolve, auto& message)
  {
    resolve.get_state().socket.async_write(
      asio::buffer(message, message.size()),
      [&](auto error, size_t)
      { (not error) ? resolve(message) : resolve(make_error(error)); }
    );
  });

  // message endpoint

  constexpr auto message = full_duplex::endpoint(
    event::read_message  = read_message,
    event::write_message = write_message
  );
}

#endif
