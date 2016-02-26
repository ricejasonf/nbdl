//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_PRIVATE_PAYLOAD_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_PRIVATE_PAYLOAD_HPP

#include<mpdef/JustifyType.hpp>
#include<def/builder/EntityMessageMeta.hpp>
#include<Message.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {
  namespace action = nbdl::message::action;
  namespace channel = nbdl::message::channel;

  // The PrivatePayload can only be sent Upstream
  // with a Create action
  template<typename MaybePrivatePayload>
  constexpr auto entityMessagePrivatePayload(MaybePrivatePayload p, action::Create, channel::Upstream)
  {
    return mpdef::justify_type(hana::decltype_(p));
  }

  // other actions don't ever get a private payload
  constexpr auto entityMessagePrivatePayload(...)
  { return hana::decltype_(hana::nothing); }
}// detail

struct EntityMessagePrivatePayload {
  template<typename A, typename M>
  constexpr auto operator()(A, M entity_message_meta) const
  {
    return detail::entityMessagePrivatePayload(
      EntityMessageMeta::privatePayload(entity_message_meta),
      EntityMessageMeta::action(entity_message_meta),
      EntityMessageMeta::channel(entity_message_meta)
    );
  }
};
constexpr EntityMessagePrivatePayload entityMessagePrivatePayload{};

}// builder
}// nbdl_def
#endif


