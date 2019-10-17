//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_IO_CONTEXT_HPP
#define NBDL_APP_IO_CONTEXT_HPP

#ifndef EMSCRIPTEN
#include <boost/asio.hpp>
namespace nbdl::app {
  using io_context = boost::asio::io_context;
}
#else
namespace nbdl::app {
  // shim for asio::io_context
  struct io_context_shim {
    void run() { }
  };

  using io_context = io_context_shim;
}
#endif

#endif
