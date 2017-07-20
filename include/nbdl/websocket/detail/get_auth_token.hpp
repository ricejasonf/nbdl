//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_DETAIL_GET_AUTH_TOKEN_HPP
#define NBDL_WEBSOCKET_DETAIL_GET_AUTH_TOKEN_HPP

#include <nbdl/auth_token.hpp>
#include <nbdl/endpoint_handler.hpp>

#include <iterator>
#include <string_view>

namespace nbdl::websocket::detail
{
  using std::string_view;
  using namespace std::string_view_literals;

  template <typename Iter>
  inline Iter check_auth_cookie(Iter begin, Iter end)
  {
    constexpr string_view nbdl_auth_sv = "nbdl_auth="sv;
    constexpr long length = nbdl_auth_sv.size();

    if (
          std::distance(begin, end) >= length
      &&  std::equal(nbdl_auth_sv.begin(), nbdl_auth_sv.end(), begin)
      )
    {
      return std::next(begin, length);
    }
    else
    {
      return std::find(begin, end, ';');
    }
  }

  template <typename Iter>
  inline auth_token_t take_cookie_value(Iter begin, Iter end)
  {
    // to semicolon or end
    Iter value_end = std::find(begin, end, ';');
    return auth_token_t{{begin, value_end}};
  }

  constexpr auto get_auth_token = [](std::string const& cookies) -> auth_token_t
  {
    // See RFC 6265
    // parse cookies, just get nbdl_auth value
    using Iter = std::string::const_iterator;
    Iter current = cookies.begin();
    Iter end = cookies.end();

    while (current != end)
    {
      current = check_auth_cookie(current, cookies.end());
      if (*current == ';')
      {
        if (std::distance(current, end) > 2)
        {
          // go past semicolon and space
          std::advance(current, 2);
        }
        else
        {
          return auth_token_t{};
        }
      }
      else
      {
        return take_cookie_value(current, end);
      }
    }

    return auth_token_t{};
  };
}

#endif
