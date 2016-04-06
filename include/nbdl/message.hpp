//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_HPP
#define NBDL_MESSAGE_HPP

#include<boost/hana.hpp>

namespace nbdl { namespace message
{

  namespace hana = boost::hana;

  /*
   * A message is just a tuple right now.
   *
   * The action and channel tags serve to
   * indicate how the message should be
   * dispatched.
   */

  namespace action {
    struct create { };
    struct read { };
    struct update { };
    struct update_raw { };
    struct delete_ { };

    struct validation_fail { };
  } // action

  namespace channel {
    struct upstream { };
    struct downstream { };
  } // channel

  // The offsets of these properties should match the
  // formation of the message in `builder::EntityMessage`.
  constexpr auto get_channel                  = hana::reverse_partial(hana::at, hana::int_c< 0 >);
  constexpr auto get_action                   = hana::reverse_partial(hana::at, hana::int_c< 1 >);
  constexpr auto get_path                     = hana::reverse_partial(hana::at, hana::int_c< 2 >);
  constexpr auto get_maybe_uid                = hana::reverse_partial(hana::at, hana::int_c< 3 >);
  constexpr auto get_maybe_is_from_root       = hana::reverse_partial(hana::at, hana::int_c< 4 >);
  constexpr auto get_maybe_payload            = hana::reverse_partial(hana::at, hana::int_c< 5 >);
  constexpr auto get_maybe_private_payload    = hana::reverse_partial(hana::at, hana::int_c< 6 >);

  // The path type is used as a key
  // in a few places.
  struct get_path_type_fn
  {
    template <typename Message>
    constexpr auto operator()(Message const& m)
    {
      return hana::type_c<std::decay_t<decltype(message::get_path(m))>>;
    }
  };

  constexpr get_path_type_fn get_path_type{};

}} // nbdl::message
#endif
