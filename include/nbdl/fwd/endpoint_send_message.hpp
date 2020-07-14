//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_SEND_MESSAGE_HPP
#define NBDL_FWD_ENDPOINT_SEND_MESSAGE_HPP

#include <nbdl/concept/Endpoint.hpp>

namespace nbdl {
  template <typename T>
  struct endpoint_send_message_impl;

  struct endpoint_send_message_fn {
    template <Endpoint Endpoint, typename Message>
    void operator()(Endpoint&, Message&&) const;
  };

  constexpr endpoint_send_message_fn endpoint_send_message{};
}

#endif
