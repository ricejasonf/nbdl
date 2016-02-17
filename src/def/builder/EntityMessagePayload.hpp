//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_PAYLOAD_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_PAYLOAD_HPP

#include<mpdef/JustifyType.hpp>
#include<Message.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {

  namespace action = nbdl::action;
  namespace channel = nbdl::channel;

  template<typename EntityType, typename Channel>
  constexpr auto entityMessagePayload(EntityType entity_type, action::Create, Channel)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename EntityType>
  constexpr auto entityMessagePayload(EntityType, action::Read, channel::Upstream)
  { return hana::decltype_(hana::nothing); }

  template<typename EntityType>
  constexpr auto entityMessagePayload(EntityType entity_type, action::Read, channel::Downstream)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename EntityType, typename Channel>
  constexpr auto entityMessagePayload(EntityType entity_type, action::UpdateRaw, Channel)
  { return mpdef::justify_type(hana::decltype_(hana::just(entity_type))); }

  template<typename EntityType, typename Channel>
  constexpr auto entityMessagePayload(EntityType, action::Delete, Channel)
  { return hana::decltype_(hana::nothing); }

  /* TODO message payloads for other possible entity actions:
   * action::Update (would use a diff which doesn't exist yet)
   */
}//detail

struct EntityMessagePayload {
  template<typename T>
  constexpr auto operator()(T entity_message_meta) const
  {
    return detail::entityMessagePayload(
      entity_message_meta.entityType(),
      entity_message_meta.action(),
      entity_message_meta.channel()
    );
  }
};
constexpr EntityMessagePayload entityMessagePayload{};

}//builder
}//nbdl_def
#endif
