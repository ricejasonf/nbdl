//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_HPP
#define NBDL_MESSAGE_HPP

namespace nbdl {

/*
 * A message is just a tuple right now.
 *
 * The action and channel tags serve to
 * indicate how the message should be
 * dispatched.
 */

  namespace action {
    struct Create { };
    struct Read { };
    struct Update { };
    struct UpdateRaw { };
    struct Delete { };
  } // action

  namespace channel {
    struct Upstream { };
    struct Downstream { };
  } // channel

} // nbdl
#endif
