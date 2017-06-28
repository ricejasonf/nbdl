//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_PARSE_HANDSHAKE_REQUEST_HPP
#define NBDL_WEBSOCKET_DETAIL_PARSE_HANDSHAKE_REQUEST_HPP

#include <nbdl/promise.hpp>

#include <algorithm>
#include <asio.hpp>
#include <boost/hana/tuple.hpp>
#include <cctype>
#include <string_view>

namespace nbdl::websocket::detail
{
  using asio::ip::tcp;
  namespace hana = boost::hana;
  using namespace std::string_view_literals;
  using std::string_view;

  struct bad_request { };

  struct handshake_info_t
  {
    std::string websocket_key;
    std::string cookies;
  };

  // This could probably use a compile time string
  constexpr auto expect_equal = [](string_view str)
  {
    std::string input_buffer(str.size(), 0);
    return nbdl::promise([=, buffer = std::move(input_buffer)](auto& resolver, auto& socket)
      mutable
    {
      asio::async_read(socket, asio::buffer(buffer, str.size()), [&](auto error_code, std::size_t)
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
          resolver.reject(bad_request{});
        }
      });
    });
  };

  constexpr auto parse_header = [](auto& handshake_info, std::string header)
  {
    const auto key_header = "sec-websocket-key"sv;
    const auto cookie_header = "cookies"sv;
    auto token_pos = std::find(header.begin(), header.end(), ':');

    std::transform(header.begin(), token_pos, header.begin(), [](auto c) { return std::tolower(c); });

    // I'm not sure it is necessary to validate characters to see if headers are valid
    if (std::equal(header.begin(), token_pos, key_header.begin(), key_header.end()))
    {
      handshake_info.websocket_key = std::string(
        std::find_if(std::next(token_pos), header.end(), [](auto c) { return c != ' '; })
      , header.end()
      );
    }
    else if (std::equal(header.begin(), token_pos, cookie_header.begin(), cookie_header.end()))
    {
      handshake_info.cookies = std::string(
        std::find_if(std::next(token_pos), header.end(), [](auto c) { return c != ' '; })
      , header.end()
      );
    }
    // else do nothing
  };

  struct read_headers
  {
    handshake_info_t handshake_info;
    std::string buffer;

    template <typename Resolver>
    void operator()( Resolver& resolver
                   , tcp::socket& socket
                   )
    {
      asio::async_read_until(
        socket
      , asio::dynamic_buffer(buffer, 256)
      , "\r\n"sv
      , [&](auto error_code, std::size_t size) noexcept
        {
          if (error_code)
          {
            resolver.reject(error_code);
          }
          else
          {
            if (size > 2)
            {
              std::string header = buffer.substr(0, size - 2);
              buffer.erase(0, size);
              parse_header(handshake_info, std::move(header));
              (*this)(resolver, socket);
            }
            else
            {
              resolver.resolve(handshake_info);
            }
          }
        }
      );
    };
  };

  // Promise that mutates a handshake_info_t
  constexpr auto parse_handshake_request = []() {
    return hana::make_tuple(
      nbdl::promise([&](auto& resolve, auto& socket) { resolve(socket); })
    , expect_equal("GET / HTTP/1.1\r\n"sv)
    , nbdl::promise(read_headers{handshake_info_t{}, std::string{}})
    );
  };
}

#endif
