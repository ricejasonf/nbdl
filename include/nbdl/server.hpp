//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERVER_HPP
#define NBDL_SERVER_HPP

#error "nbdl::server is a WIP"

#include <nbdl/ext/beast_ws.hpp>

#include <full_duplex.hpp>
#include <optional>
#include <string>
#include <utility>

namespace nbdl
{
  // detail
  // maybe store connections in Context
  template <typename Context, typename Socket>
  struct session
  {
    session(Context c, Socket s)
      : context(c)
      , socket(std::move(s))
    { }

    Context context;
    Socket socket;
  };

  auto create_endpoint = [](auto context)
  {
    return full_duplex::promise([=](auto socket)
    {
      using full_duplex::endpoint;
      using full_duplex::event;

      return full_duplex::endpoint_open(
        std::move(state)
      , std::forward<Queue>(queue)
      , full_duplex::endpoint_compose(
        , endpoint(
#if 0
            event::init           = /* TODO init stuff
                                       - Verify API
                                       - Authenticate/Resolve ACL
                                       - Add self to connection list
                                       - ???
                                       - profit
                                    */
#endif
          , event::read_message   = read_message
          , event::write_message  = write_message
          , event::error          = /* - Delete from connection list
                                    */
          )
        , endpoint(std::forward<Events>(events))
        )
      )
    });
  };

  // 
  template <typename Context, typename Acceptor>
  void run_server_loop(Context context, Acceptor acceptor)
  {
    full_duplex::run_async_loop(
      accept_socket(std::move(acceptor))
    , 
    );
  }

  // Consumer

  template <typename Endpoint, typename Context>
  struct actor_type<server_tag<Endpoint>, Context>
  {
    using type = server_impl<Context, Endpoint>;
  };

  template <>
  struct send_downstream_message<server_tag<Endpoint>>
  {
    template <typename Consumer, typename Message>
    static void apply(Consumer& c, Message&& m)
    {
      c.endpoint.write_message(std::forward<Message>(m));
    }
  };
}

#endif
