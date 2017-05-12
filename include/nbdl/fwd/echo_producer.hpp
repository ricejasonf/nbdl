//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ECHO_PROVIDER_HPP
#define NBDL_FWD_ECHO_PROVIDER_HPP

namespace nbdl
{
  // tag
  struct echo_producer { };

  template <typename PushApi>
  struct echo_producer_impl;
} // nbdl

#endif
