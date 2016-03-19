//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_PAYLOAD_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_PAYLOAD_HPP

#include<mpdef/JustifyType.hpp>
#include<nbdl/message.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {

  namespace action = nbdl::message::action;
  namespace channel = nbdl::message::channel;

  template<typename A, typename E, typename Channel>
  constexpr auto entity_message_payload(A, E entity_type, action::create, Channel)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename A, typename E>
  constexpr auto entity_message_payload(A, E, action::read, channel::upstream)
  { return hana::decltype_(hana::nothing); }

  template<typename A, typename E>
  constexpr auto entity_message_payload(A, E entity_type, action::read, channel::downstream)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename A, typename E, typename Channel>
  constexpr auto entity_message_payload(A, E entity_type, action::update_raw, Channel)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename A, typename E, typename Channel>
  constexpr auto entity_message_payload(A, E, action::delete_, Channel)
  { return hana::decltype_(hana::nothing); }

  /* TODO message payloads for other possible entity actions:
   * action::Update (would use a diff which doesn't exist yet)
   */
}//detail

struct entity_message_payload_fn {
  template<typename A, typename E>
  constexpr auto operator()(A access_point, E entity_message_meta) const
  {
    return detail::entity_message_payload(
      access_point,
      entity_message_meta::entity_type(entity_message_meta),
      entity_message_meta::action(entity_message_meta),
      entity_message_meta::channel(entity_message_meta)
    );
  }
};
constexpr entity_message_payload_fn entity_message_payload{};

}//builder
}//nbdl_def
#endif
