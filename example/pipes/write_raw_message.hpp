//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_WRITE_RAW_MESSAGE_HPP
#define NBDL_EXAMPLE_WRITE_RAW_MESSAGE_HPP

#include <asio.hpp>
#include <cstdint>
#include <nbdl.hpp>

namespace example
{
  struct write_raw_message_fn
  {
    using tcp = asio::ip::tcp;
    tcp::socket& socket;
    std::string body;

    write_raw_message_fn(tcp::socket& socket)
      : socket(socket)
    { }

    template <typename Resolve, typename Body>
    void operator()(Resolve&& resolve, Body body_)
    {
      const uint32_t length = body_.size();
      const unsigned char length_as_bytes[4] = {
        static_cast<unsigned char>(length >> 24)
      , static_cast<unsigned char>(length >> 16)
      , static_cast<unsigned char>(length >> 8)
      , static_cast<unsigned char>(length)
      };

      body = std::move(body_).insert(0, reinterpret_cast<char const*>(length_as_bytes), 4);
      asio::async_write(socket, asio::buffer(body, body.size()),
        [resolve](std::error_code error, std::size_t)
        {
          if (!error)
          {
            resolve();
          }
          else
          {
            resolve.reject(error);
          }
        });
    }
  };

  auto write_raw_message = [](asio::ip::tcp::socket& socket)
  {
    return nbdl::promise(write_raw_message_fn(socket));
  };
}

#endif
