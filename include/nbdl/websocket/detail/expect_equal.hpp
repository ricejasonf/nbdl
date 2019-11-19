//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_EXPECT_EQUAL_HPP
#define NBDL_WEBSOCKET_DETAIL_EXPECT_EQUAL_HPP

#include <nbdl/promise.hpp>
#include <nbdl/websocket/event.hpp>

#include <algorithm>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <string_view>

namespace nbdl::websocket::detail
{
  namespace asio = boost::asio;
  using std::string_view;

  constexpr auto expect_equal = [](string_view str)
  {
    std::string input_buffer(str.size(), 0);
    return nbdl::promise([=, buffer = std::move(input_buffer)](auto& resolver, auto& socket)
      mutable
    {
      asio::async_read(socket, asio::buffer(buffer, str.size()),
                       [&](std::error_code error_code, std::size_t)
      {
        if (error_code)
        {
          resolver.reject(error_code);
        }
        else if (std::equal(str.begin(), str.end(), buffer.begin()))
        {
          resolver.resolve(socket);
        }
        else
        {
          // TODO maybe just use an error_code
          resolver.reject(event::bad_request);
        }
      });
    });
  };
}

#endif
