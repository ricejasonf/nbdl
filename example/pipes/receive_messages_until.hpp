//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_RECEIVE_MESSAGES_UNTIL_HPP
#define NBDL_EXAMPLE_RECEIVE_MESSAGES_UNTIL_HPP

#include <asio.hpp>
#include <nbdl.hpp>
#include <nbdl/binder/jsoncpp.hpp>
#include <utility>

#include "read_raw_message.hpp"

namespace example
{
  template <typename Message, typename Terminate, typename Action>
  struct receive_messages_until_fn
  {
    using tcp = asio::ip::tcp;
    Action action;

    receive_messages_until_fn(Action action)
      : action(std::move(action))
    { }

    template <typename Resolve>
    void operator()(Resolve&& resolve, tcp::socket& socket)
    {
      nbdl::pipe(
        read_raw_message(socket)
      , [&, resolve](std::string&& msg_json)
        {
          std::cout << "RAW MESSAGE: " << msg_json << '\n';
          Message message;
          nbdl::binder::jsoncpp::from_string(std::move(msg_json), message);

          // This is where the enclosed message is printed.
          message.match(
            [](nbdl::unresolved) { /* uhhh */ }
          , [&](auto const& x) { action(x); }
          );

          message.match(
            [&](Terminate) { socket.close(); resolve(message); }
          , [&](auto&&)
            {
              operator()(resolve, socket);
            }
          );
        }
      , nbdl::catch_([resolve](auto&& error) { resolve.reject(error); })
      )();
    }
  };

  auto receive_messages_until = [](auto MessageType, auto TerminateType, auto&& action)
  {
    using Message   = typename decltype(MessageType)::type;
    using Terminate = typename decltype(TerminateType)::type;
    using Action    = decltype(action);

    return nbdl::promise(receive_messages_until_fn<Message, Terminate, std::decay_t<Action>>(
      std::forward<decltype(action)>(action)
    ));
  };
}

#endif
