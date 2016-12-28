//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_CONNECT_HPP
#define NBDL_EXAMPLE_CONNECT_HPP

#include <asio.hpp>
#include <cstdint>
#include <nbdl.hpp>

namespace example
{
  struct attempts
  {
    int value;
  };

  using attempts_failed = attempts;

  struct port
  {
    short value; 
  };

  struct connect_fn
  {
    using tcp = asio::ip::tcp;
    tcp::socket& socket;
    tcp::endpoint endpoint;
    attempts attempts_;
    int attempts_failed;

    connect_fn(tcp::socket& socket, port port_, attempts attempts_ = attempts{10})
      : socket(socket)
      , endpoint(tcp::endpoint(tcp::v4(), port_.value))
      , attempts_(attempts_)
      , attempts_failed(0)
    { }

    template <typename Resolve, typename Reject, typename ...Args>
    void operator()(Resolve&& resolve, Reject&& reject, Args&& ...)
    {
      socket.async_connect(endpoint, [&, resolve, reject](asio::error_code error)
      {
        if (!error)
        {
          resolve(); 
        }
        else if (attempts_failed++ >= attempts_.value)
        {
          reject(attempts_);
        }
        else
        {
          // try again
          operator()(resolve, reject);
        }
      });
    }
  };

  auto connect = [](asio::ip::tcp::socket& socket, port port_, attempts attempts_ = attempts{10})
  {
    return nbdl::promise(connect_fn(socket, port_, attempts_));
  };
}

#endif
