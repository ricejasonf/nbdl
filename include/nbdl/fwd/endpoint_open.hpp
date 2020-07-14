//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_OPEN_HPP
#define NBDL_FWD_ENDPOINT_OPEN_HPP

#include <nbdl/concept/EndpointInfo.hpp>

namespace nbdl {
  template <typename T>
  struct endpoint_open_impl;

  struct endpoint_open_fn {
    template <EndpointInfo EndpointInfo, typename SendQueue, typename EndpointHandler>
    auto operator()(EndpointInfo&&, SendQueue&&, EndpointHandler&&) const;
  };

  constexpr endpoint_open_fn endpoint_open{};
}

#endif
