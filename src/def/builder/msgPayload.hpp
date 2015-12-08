//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_MSG_PAYLOAD_HPP
#define NBDL_DEF_MSG_PAYLOAD_HPP

#include "../builder.hpp"
#include "./Path.hpp"
#include "./AccessPoint.hpp"

namespace nbdl_def {
namespace builder {

//default should be the Entity or Diff<Entity>
template<typename MsgBuilder_, typename Action, typename Channel>
auto msgPayload(MsgBuilder_ const& msg_builder, Action, Channel)
{
  using Path_ = typename decltype(path(msg_builder.access_point))::type;
  using Entity_ = typename Path_::Entity;
  /* todo implement Diff
  return hana::if_(
    AccessPoint(msg_builder.ctx, msg_builder.access_point)
      .actionUsesDiff(action),
    hana::just(hana::type_c<Diff<Entity_>>),
    hana::just(hana::type_c<Entity_>));
  */
  return hana::just(entityFromAccessPoint(msg_builder.access_point));
}

template<typename MsgBuilder_>
auto msgPayload(MsgBuilder_, action::Read, channel::Upstream)
{
  return hana::nothing;
}

template<typename MsgBuilder_>
auto msgPayload(MsgBuilder_, action::Create, channel::DownStream)
{
  return hana::nothing;
}

template<typename Channel, typename MsgBuilder_>
auto msgPayload(MsgBuilder_, action::Delete, Channel)
{
  return hana::nothing;
}

template<typename Action, typename Channel, typename MsgBuilder_>
auto msgPayloadPrivate(MsgBuilder_, Action, Channel)
{
  return hana::nothing;
}

template<typename MsgBuilder_>
auto msgPayloadPrivate(MsgBuilder_, action::Create, channel::Upstream)
{
  //todo access_point specifies PrivatePayload
  return hana::nothing;
}

}//builder
}//nbdl_def
#endif
