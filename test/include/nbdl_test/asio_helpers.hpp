//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_ASIO_HELPERS_HPP
#define NBDL_TEST_ASIO_HELPERS_HPP

#include <asio.hpp>
#include <nbdl/variant.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/tap.hpp>

namespace nbdl_test
{
  using asio::ip::tcp;

  inline tcp::endpoint endpoint() { return tcp::endpoint(tcp::v4(), 8585); }

  // connect

  constexpr auto connect = []
  {
    return nbdl::promise([](auto& resolver, tcp::socket& socket)
    {
      socket.async_connect(endpoint(), [&](auto error)
      {
        if (error)
        {
          resolver.reject(error);
        }
        else
        {
          resolver.resolve(socket); 
        }
      });
    });
  };

  constexpr auto accept = []
  {
    nbdl::optional<tcp::acceptor> h{};

    return nbdl::promise([acceptor_holder = std::move(h)](auto& resolver
                                                        , tcp::socket& socket)
                                                          mutable
    {
      tcp::acceptor acceptor(socket.get_io_service(), endpoint());
      acceptor.async_accept(socket, [&](auto error)
      {
        if (error)
        {
          resolver.reject(error);
        }
        else
        {
          resolver.resolve(socket); 
        }
      });
      acceptor_holder = std::move(acceptor);
    });
  };

  constexpr auto send_data = [](auto data)
  {
    return nbdl::promise([data = std::move(data)](auto& resolver, tcp::socket& socket)
    {
      asio::async_write(socket, asio::buffer(data), [&](auto error, std::size_t)
      {
        if (error)
        {
          resolver.reject(error);
        }
        else
        {
          resolver.resolve(socket); 
        }
      });
    });
  };
}

#endif
