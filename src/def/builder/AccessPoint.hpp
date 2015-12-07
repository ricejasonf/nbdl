//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ACCESS_POINT_HPP
#define NBDL_DEF_ACCESS_POINT_HPP

#include "../builder.hpp"

namespace nbdl_def {
namespace builder {

//no actual AccessPoint object exists
//but these are some helper functions
//for getting meta information about them

template<typename ContextDef_, typename AccessPointDef_>
class AccessPoint
{
  using ContextDef = ContextDef_;
  using AccessPointDef = AccessPointDef_;

  ContextDef const& ctx;
  AccessPointDef const& access_point;

  AccessPoint(ContextDef const& c, AccessPointDef const& a) :
    ctx(c),
    access_point(a)
  {}

  template<typename Action>
  constexpr auto actionUsesDiff(Action)
  {
    return hana::bool_c<false>;  
  }
  //todo return true for create and update that don't specify NoDiff

};

}//builder
}//nbdl_def
#endif
