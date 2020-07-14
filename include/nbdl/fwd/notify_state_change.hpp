//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_NOTIFY_STATE_CHANGE_HPP
#define NBDL_FWD_NOTIFY_STATE_CHANGE_HPP

#include <nbdl/concept/StateConsumer.hpp>

namespace nbdl {
  template <typename T>
  struct notify_state_change_impl;

  struct notify_state_change_fn {
    template <StateConsumer StateConsumer, typename Path>
    constexpr auto operator()(StateConsumer&&, Path&&) const;
  };

  constexpr notify_state_change_fn notify_state_change{};
}

#endif

