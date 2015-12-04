//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ACCESS_POINT_HPP
#define NBDL_DEF_ACCESS_POINT_HPP

#include "../builder.hpp"
#include "../directives.hpp"

namespace nbdl_def {
namespace builder {

//no actual AccessPoint object exists
//but these are some helper functions
//for getting meta information about them

template<typename AccessPointDef>
constexpr auto doesAccessPointHaveLocalVersion(AccessPointDef)
{
  return hana::bool_c<false>;  
  //todo stores will have special 'LocalVersion' objects if this is true
  //todo it also means create and update messages will have a uuid
}

template<typename Action, typename AccessPointDef>
constexpr auto doesActionUseDiff(Action, AccessPointDef)
{
  return hana::bool_c<false>;  
}
//todo return true for create and update that don't specify NoDiff

}//builder
}//nbdl_def
#endif
