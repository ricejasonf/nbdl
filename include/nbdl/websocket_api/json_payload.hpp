//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_API_JSON_PAYLOAD_HPP
#define NBDL_WEBSOCKET_API_JSON_PAYLOAD_HPP

#include <nbdl/js.hpp>

namespace nbdl::websocket_api
{
  // This represents a js_val that should point
  // to a javascript value to be converted to JSON text.
  using json_payload = nbdl::js::val;
}

#endif
