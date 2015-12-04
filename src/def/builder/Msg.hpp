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

/*
 * DOWNSTREAM only messages:
 *  Read
 *  NotFound
 *  Error ??
 * 
 * UPSTREAM only messages:
 *  ReadRequest
 *  CreateUpstreamOnly 
 *  UpdateUpstreamOnly
 *  DeleteUpstreamOnly
 *  AccessControlError
 *
 * UPSTREAM/DOWNSTREAM:
 *  Create
 *  Update
 *  Delete  
 */

/*
 * Msg
 *
  ((
    Path,
    (Uuid|!)
    Entity,
    (CreateInfo|!) <-'create' messages only
  ), Storage)
 */
template<typename MsgMeta>
auto msgStorage(MsgMeta meta)
{
  //todo filter emtpies and make tuple of denested types
}
/*
 * - messages always have a path
 * - messages will have uuid unless UpstreamOnly is set for that action
 * - if a 'create' has a uuid then store it in a special store for that entity
 * - others with uuid will modify the object in the store (update/delete)
 * - create/update will be diff unless NoDiff is specified
 * - 'create' messages may have an additional CreateInfo object for secret info
 *   that is not part of the entity's data model
 *
 * - structure
 *   pair:
 *    first: [ path_type, action_type, uuid_type, payload_1_type, payload_2_type ] 
 *    second: transform(filter(first, nonEmptyTypes), inner_type)
 *
 */

template<typename Action, typename AccessPoint>
auto msgMeta(Action action, AccessPoint access_point)
{
  auto path = path(access_point);
  return hana::make_tuple(
    action,
    path(access_point)
    msgUuid(action, access_point)
  );
}

template<typename Action, typename AccessPoint>
auto msg(Action action, AccessPoint access_point)
{
  msgMeta(action
  return hana::make_pair(
}

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
