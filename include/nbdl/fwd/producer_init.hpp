//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_PRODUCER_INIT_HPP
#define NBDL_FWD_PRODUCER_INIT_HPP

#include <nbdl/concept/Producer.hpp>

namespace nbdl {
  template <typename T>
  struct producer_init_impl;

  //
  // A hook to allow producers to do initialization stuffs
  // and send messages after the context's components have
  // all been constructed.
  struct producer_init_fn {
    template <Producer Producer>
    constexpr void operator()(Producer&) const;
  };

  constexpr producer_init_fn producer_init{};
}

#endif
