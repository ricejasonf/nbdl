//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_MSG_HPP
#define NBDL_DEF_BUILDER_MSG_HPP

#include "../../Make.hpp"
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

  const ContextDef ctx;
  const AccessPointDef access_point;

  constexpr Msg(AccessPointBuilder a)
    ctx(decltype(a.ctx){}),
    access_point(decltype(a.access_point){})
  {}

  /*
   * UPSTREAM/DOWNSTREAM:
   *  Create
   *  Read
   *  Update
   *  Delete  
   *
   * todo
   * DOWNSTREAM only messages:
   *  NotFound
   *  ValidationFail
   *  Error ??
   * 
   */

  template<typename MsgMeta>
  auto storage(MsgMeta meta_) const
  {
    //todo filter out nothings and make tuple of denested types
    return hana::unpack(hana::filter(meta_, hana::is_just),
      [](auto... x) {
        return hana::type_c<decltype(hana::make_tuple(typename decltype(*x)::type...))>;
      });
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
  auto path(Action action, Channel channel) const
  {
    return builder::path(access_point);
  }

  template<typename AccessPoint>
  auto path(action::Create, channel::Upstream) const
  {
    using Path_ = typename decltype(path(access_point))::type;
    return Path_::Parent::createChildType(
      hana::type_c<typename Path_::Entity>,
      hana::type_c<void>); //no point in specifying a value that we dont know yet
  }

  template<typename Action>
  auto requestId(Action action) const
  {
    return hana::if_(
      entityHasLocalVersion(ctx, entityFromAccessPoint(access_point))
      && action ^hana::in^ hana::tuple<action::Create, action::Update>{}
      , hana::just(hana::type_c<MsgId>), hana::nothing);
  }

  template<typename Action, typename Channel>
  auto payload(Action&& action, Channel&& channel) const
  {
    return hana::overload_linearly(
        [](action::Read, channel::Upstream) {
          return hana::nothing;
        },
        [](action::Create, channel::DownStream) {
          return hana::nothing;
        },
        [](action::Delete, auto) {
          return hana::nothing;
        },
        [&](auto&& /*action*/, auto) {
          auto entity = entityFromAccessPoint(access_point);
          /* todo implement Diff
          return hana::if_(
            AccessPoint(msg_builder.ctx, msg_builder.access_point)
              .actionUsesDiff(std::forward<Action>(action)),
            hana::just(diff(entity)),
            hana::just(entity));
          */
          return hana::just(entity);
        }
      )(std::forward<Action>(action), std::forward<Channel>(channel));
  }

  template<typename Action, typename Channel>
  auto meta(Action action, Channel channel) const
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
  auto build(Action action, Channel channel)
  {
    auto meta_ = meta(action, channel);
    return hana::type_c<decltype(hana::make_pair(
      meta_,
      typename decltype(storage(meta_))::type))>;
  }

  //builds all CRUD messages for AccessPoint
  auto build()
  {
    return hana::unpack(hana::make_tuple(
      action::Create,
      action::Read,
      action::Update,
      action::Delete)
    | [](auto t) {
        return hana::make_tuple(
          hana::make_pair(t, channel::Upstream),
          hana::make_pair(t, channel::Downstream)
        );
      },
    [](auto... combo) {
      return hana::make_tuple(
        hana::unpack(combo, [](auto action, auto channel) {
          return build(action, channel);
        })...);
    });
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
#endif
