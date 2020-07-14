//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_SEND_CLOSE_HPP
#define NBDL_FWD_ENDPOINT_SEND_CLOSE_HPP

#include <nbdl/concept/Endpoint.hpp>

namespace nbdl {
  template <typename T>
  struct endpoint_send_close_impl;

  struct endpoint_send_close_fn {
    template <Endpoint Endpoint>
    void operator()(Endpoint&) const;
  };

  constexpr endpoint_send_close_fn endpoint_send_close{};
}

#endif
