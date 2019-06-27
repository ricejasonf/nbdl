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

namespace nbdl::ext::beast_ws
{
  namespace asio  = boost::asio;
  namespace beast = boost::beast;
  namespace event = full_duplex::event;
  using full_duplex::make_error;
  using full_duplex::promise;
  using full_duplex::promise_lift;

  using asio::ip::tcp;
  using stream_t = beast::websocket::stream<tcp::socket>;

  // accept -/ Promise that resolves a
  // stream_t
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
      auto& stream = resolve.get_state().stream();
      stream.next_layer() = std::move(socket);

      stream.async_accept([&resolve](boost::system::error_code error) {
        (not error) ? resolve(std::move(resolve.get_state().stream())) :
                      resolve(make_error(error));
      });
      // TODO get session cookie somehow
    })
  );

  // connect - currently used for integration testing
  //           service may be a port number
  constexpr auto connect = [](auto& io,
                              std::string host,
                              std::string service) {
    tcp::resolver resolver{io};
    return full_duplex::do_(
      promise([resolver = std::move(resolver),
               host     = std::move(host),
               service  = std::move(service)]
        (auto& resolve, auto&&) mutable {
        resolver.async_resolve(tcp::v4(), host, service, 
                               [&](auto error, auto results) {
          // the result range is guaranteed to be non-empty unless
          // there is an error
          (not error) ? resolve(std::move(*(results.begin()))) :
                        resolve(make_error(error));
        });
      }),
      promise([](auto& resolve, auto entry) {
          auto& socket = resolve.get_state().socket();
          socket.async_connect(entry.endpoint(),
                               [&resolve](auto error) {
              (not error) ? resolve(full_duplex::void_input) :
                            resolve(make_error(error));
          });
      }),
      promise([](auto& resolve, auto const&) {
          auto& stream = resolve.get_state().stream();
          stream.async_handshake(boost::string_view("localhost"),
                                 boost::string_view("/"),
                                 [&](boost::system::error_code error) {
            (not error) ? resolve(full_duplex::void_input) :
                          resolve(make_error(error));
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
      resolve.get_state().stream().async_read(buffer, [&](auto error, size_t)
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

  constexpr auto read_message = [](auto&& /* self */) {
    return promise(read_message_fn{});
  };

  // write_message

  constexpr auto write_message = promise([](auto& resolve, auto& message)
  {
    resolve.get_state().stream().async_write(
      asio::buffer(message, message.size()),
      [&](auto error, size_t)
      { (not error) ? resolve(message) : resolve(make_error(error)); }
    );
  });

  // message_endpoint

  constexpr auto message_endpoint = full_duplex::endpoint(
    event::read_message  = read_message,
    event::write_message = write_message
  );
}

#endif
