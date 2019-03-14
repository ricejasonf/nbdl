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
#include <string>
#include <string_view>
#include <utility>

namespace nbdl::beast_ws
{
  namespace asio  = boost::asio;
  namespace beast = boost::beast;
  using asio::ip::tcp;

  struct websocket_acceptor
  {
    tcp::acceptor acceptor;

    template <typename ...Args>
    websocket_acceptor(Args&& ...args)
      : impl(std::forward<Args>(args)...)
    { }
  };

  // Returns a Promise that resolves a
  // socket or an error
  //
  // Could be part of a Socket concept
  auto accept_socket(websocket_acceptor ws)
  {
    using full_duplex::promise;
    using full_duplex::promise_lift;
    using full_duplex::make_error;

    return full_duplex::do_(
      promise_lift(std::pair{
        std::move(ws.acceptor)
      , beast::websocket::stream<tcp::socket>(io)
      })
    , promise([](auto& resolve, auto& pair)
      {
        auto& [acceptor, socket] = pair;
        acceptor.async_accept(socket, [&](auto error)
        {
          (not error) ? resolve(pair) :
                        resolve(make_error(error));
        });
      })
    , promise([](auto& resolve, auto& pair)
      {
        auto& [acceptor, socket] = pair;
        socket.async_accept([&](auto error)
        {
          (not error) ? resolve(std::move(socket)) :
                        resolve(make_error(error));
        });
      })
    );
  }

  // socket_state

  struct socket_state
  {
    beast::websocket::stream<tcp::socket> socket;
  };

  // connect_state

  struct connect_state : socket_state
  {
    connect_state(asio::io_service& io,
                  std::string host,
                  std::string target
                  unsigned short port)
      : socket_state(beast::websocket(io))
      , host(std::move(host))
      , target(std::move(target))
      , endpoint(tcp::v4(), port)
    { }

    std::string host;
    std::string target;
    beast::websocket::stream<tcp::socket> socket;
    tcp::endpoint endpoint;
  };
}

namespace nbdl::beast_ws::detail
{
  using full_duplex::do_;
  using full_duplex::make_error;
  using full_duplex::promise;


#if 0
  constexpr auto accept = promise([](auto& resolve, auto&&)
  {
    // This does the websocket handshake
    resolve.get_state().ws.async_accept([&](auto error)
    {
      (not error) ? resolve(full_duplex::void_input) :
                    resolve(make_error(error));
    });
  });
#endif

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

#if 0
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
#endif

namespace nbdl
{
  // Producer

  template <typename Context>
  struct actor_type<beast_ws::connect_state, Context>
  {
    using type = beast_ws::detail::client_impl<Context>;
  };

  template <>
  struct send_downstream_message<beast_ws::connect_state>
  {
    template <typename Consumer, typename Message>
    static void apply(Consumer& c, Message&& m)
    {
      c.write_message(std::forward<M>(m));
    }
  };

  // Consumer

  template <typename Context>
  struct actor_type<beast_ws::accept_state, Context>
  {
    using type = beast_ws::detail::server_impl<Context>;
  };

  template <>
  struct send_downstream_message<beast_ws::accept_state>
  {
    template <typename Consumer, typename Message>
    static void apply(Consumer& c, Message&& m)
    {
      c.write_message(std::forward<M>(m));
      /*
      // marshalling/filtering happens in the endoints'
      // write_message customization
      //
      // Still, a way to allow the user to map these would
      // be good
      for (auto& conn : c.connections())
      {
        conn.write_message(std::forward<M>(m));
      }
      */
    }
  };
}

#endif
