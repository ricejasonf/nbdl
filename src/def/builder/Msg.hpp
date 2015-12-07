//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_MSG_HPP
#define NBDL_DEF_MSG_HPP

#include "../builder.hpp"
#include "../directives.hpp"
#include "./Path.hpp"
#include "./AccessPoint.hpp"

namespace nbdl_def {
namespace builder {

template<typename AccessPointBuilder>
struct Msg
{
  using ContextDef = typename AccessPointBuilder::ContextDef;
  using AccessPointDef = typename AccessPointBuilder::AccessPointDef;

  ContextDef const& ctx;
  AccessPointDef const& access_point;

  Msg(AccessPointBuilder const& a)
    ctx(a.ctx),
    access_point(a.access_point)
  {}

  /*
   * UPSTREAM/DOWNSTREAM:
   *  Create
   *  Read
   *  Update
   *  Delete  
   *
   * DOWNSTREAM only messages:
   *  NotFound
   *  ValidationFail
   *  Error ??
   * 
   */

  template<typename MsgMeta>
  auto storage(MsgMeta meta)
  {
    //todo filter out nothings and make tuple of denested types
  }

  /*
   * - messages always have a path
   * - messages will have RequestId unless UpstreamOnly is set for that action
   * - if a 'create' has a RequestId then store it in a special store for that entity
   * - others with RequestId will modify the object in the store (update/delete)
   * - create/update will be diff unless NoDiff is specified
   * - 'create' messages may have an additional CreateInfo object for secret info
   *   that is not part of the entity's data model
   *
   * - structure
   *   pair:
   *    first: [ path_type, action_type, RequestId_type, payload_type, payload_private_type ] 
   *    second: transform(filter(first, nonEmptyTypes), inner_type)
   *
   */

  template<typename Action, typename Channel>
  auto path(Action action, Channel channel)
  {
    return builder::path(access_point);
  }

  template<typename AccessPoint>
  auto path(action::Create, channel::Upstream)
  {
    using Path_ = typename decltype(path(access_point))::type;
    return Path_::Parent::createChildType(
      hana::type_c<typename Path_::Entity>,
      hana::type_c<void>); //no point in specifying a value that we dont know yet
  }

  template<typename Action>
  auto requestId(Action action)
  {
    return hana::if_(
      entityHasLocalVersion(ctx, entityFromAccessPoint(access_point))
      && action ^hana::in^ hana::tuple<action::Create, action::Update>{}
      , hana::just(hana::type_c<MsgId>), hana::nothing);
  }

  template<typename Action, typename Channel>
  auto meta(Action action, Channel channel)
  {
    return hana::make_tuple(
      hana::just(action),
      hana::just(channel),
      path(action, channel),
      requestId(action),
      msgPayload(this, action, channel);
      msgPayloadPrivate(this, action, channel);
    );
  }

  template<typename Action, typename Channel>
  auto msg(Action action, Channel channel)
  {
    msgMeta(action
    return hana::make_pair(
  }
};

//messages that go upstream
template<typename T>
auto upstreamMsgs(T access_point)
{
  hana::unpack(
    meta::findByTag(access_point, tag::Actions),
    [](auto... actions) {
        return hana::transform(
          hana::filter(
            hana::make_tuple(something_that_returns_an_optional(actions)...),
            hana::is_just), 
          hana::from_just);
    });
}

template<typename T>
constexpr auto downstreamMsgs(T access_point)
{
  
}

}//builder
}//nbdl_def
