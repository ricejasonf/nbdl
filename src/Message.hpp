//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_HPP
#define NBDL_MESSAGE_HPP

#include<boost/hana.hpp>

namespace nbdl {
namespace message {

namespace hana = boost::hana;

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

    struct ValidationFail { };
  } // action

  namespace channel {
    struct Upstream { };
    struct Downstream { };
  } // channel

  // The offsets of these properties should match the
  // formation of the message in `builder::EntityMessage`.
  constexpr auto getChannel               = hana::reverse_partial(hana::at, hana::int_c< 0 >);
  constexpr auto getAction                = hana::reverse_partial(hana::at, hana::int_c< 1 >);
  constexpr auto getPath                  = hana::reverse_partial(hana::at, hana::int_c< 2 >);
  constexpr auto getMaybeIsFromRoot       = hana::reverse_partial(hana::at, hana::int_c< 3 >);
  constexpr auto getMaybeUid              = hana::reverse_partial(hana::at, hana::int_c< 4 >);
  constexpr auto getMaybePayload          = hana::reverse_partial(hana::at, hana::int_c< 5 >);
  constexpr auto getMaybePrivatePayload   = hana::reverse_partial(hana::at, hana::int_c< 6 >);

} // message
} // nbdl
#endif
