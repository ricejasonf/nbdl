//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_CLIENT_CONNECTION_HPP
#define NBDL_APP_CLIENT_CONNECTION_HPP

#ifdef EMSCRIPTEN
#include <nbdl/app/client_connection_js.hpp>
#else // NOT EMSCRIPTEN

#include <nbdl.hpp>
#include <full_duplex.hpp>

#include <string>

namespace nbdl
{
  // TODO implement Beast Websocket connection interface
}

#endif // EMSCRIPTEN
#endif
