//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SEND_DOWNSTREAM_MESSAGE_HPP
#define NBDL_FWD_SEND_DOWNSTREAM_MESSAGE_HPP

#include <nbdl/concept/Consumer.hpp>

namespace nbdl {
  template <typename T>
  struct send_downstream_message_impl;

  struct send_downstream_message_fn {
  template <Consumer T, DownstreamMessage Message>
    constexpr auto operator()(T&&, Message&&) const;
  };

  constexpr send_downstream_message_fn send_downstream_message{};
}

#endif

