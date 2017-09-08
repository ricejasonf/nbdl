//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MESSAGE_HPP
#define NBDL_FWD_MESSAGE_HPP

namespace nbdl::message
{
  /*
   * A message is just a tuple.
   *
   * The action and channel tags serve to
   * indicate how the message should be
   * dispatched.
   */

  namespace action
  {
    struct create { };
    struct read { };
    struct update { };
    struct delete_ { };

    struct validation_fail { };
  }

  namespace channel
  {
    struct upstream { };
    struct downstream { };
  }
}

#endif
