//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBSOCKET_API_CLIENT_ENDPOINT_HPP
#define NBDL_FWD_WEBSOCKET_API_CLIENT_ENDPOINT_HPP

#include <nbdl/auth_token.hpp>

#include <emscripten.h>
#include <string>

namespace nbdl::websocket_api
{
  struct origin_t
  {
    std::string value;
  };

  struct client_endpoint
  {
    std::string url;
    auth_token_t auth_token;
    origin_t origin;
  };
}

#endif
