//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_ACCEPT_HPP
#define NBDL_EXAMPLE_ACCEPT_HPP

#include <asio.hpp>
#include <nbdl.hpp>

#include "connect.hpp" // for example::port

namespace example
{

  // accepts a single connection
  struct accept_fn
  {
    using tcp = asio::ip::tcp;
    tcp::endpoint endpoint;
    tcp::socket socket;
    nbdl::optional<tcp::acceptor> acceptor;

    accept_fn(asio::io_service& io, port port_)
      : endpoint(tcp::v4(), port_.value)
      , socket(io)
    { }

    template <typename Resolve, typename ...Args>
    void operator()(Resolve&& resolve, Args&& ...)
    {
      tcp::acceptor acceptor_(socket.get_io_service(), endpoint);

      acceptor_.async_accept(socket, [&, resolve](std::error_code error)
      {
        if (!error)
        {
          resolve(socket);
        }
        else
        {
          resolve.reject(error);
        }
      });

      acceptor = std::move(acceptor_);
    }
  };

  auto accept = [](asio::io_service& io, port port_)
  {
    return nbdl::promise(accept_fn(io, port_));
  };
}

#endif
