//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ECHO_PRODUCER_HPP
#define NBDL_FWD_ECHO_PRODUCER_HPP

namespace nbdl
{
  template <typename Context>
  struct echo_producer_impl;

  struct echo_producer
  {
    template <typename Context>
    using actor_impl = echo_producer_impl<Context>;
  };
}

#endif
