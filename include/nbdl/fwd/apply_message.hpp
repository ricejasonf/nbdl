//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_APPLY_MESSAGE_HPP
#define NBDL_FWD_APPLY_MESSAGE_HPP

#include <nbdl/concept/NetworkStore.hpp>

namespace nbdl {
  namespace hana = boost::hana;

  struct apply_message_fn {
    // returns true if the contained state changed
    template <NetworkStore Store, typename Message>
    constexpr auto operator()(Store&&, Message&&) const;
  };

  constexpr apply_message_fn apply_message{};
}

#endif
