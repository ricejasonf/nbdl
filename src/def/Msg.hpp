//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_MSG_HPP
#define NBDL_DEF_MSG_HPP

namespace nbdl_ddl {
namespace def {

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
    MsgType
    Path,
    Entity,
  ), Storage)
 */

template<typename T>
constexpr auto createMsgs(T access_point)
{

}

}//def
}//nbdl_ddl
