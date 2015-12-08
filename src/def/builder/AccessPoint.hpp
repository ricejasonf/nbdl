//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ACCESS_POINT_HPP
#define NBDL_DEF_BUILDER_ACCESS_POINT_HPP

#include "../../Make.hpp"
#include "../builder.hpp"

namespace nbdl_def {
namespace builder {

template<typename ContextDef_, typename AccessPointDef_>
class AccessPoint
{
  using ContextDef = ContextDef_;
  using AccessPointDef = AccessPointDef_;

  const ContextDef ctx;
  const AccessPointDef access_point;

  constexpr AccessPoint(ContextDef&& c, AccessPointDef&& a) :
    ctx(std::forward<ContextDef>(c)),
    access_point(std::forward<AccessPointDef>(a))
  {}

  template<typename Action>
  constexpr auto actionUsesDiff(Action)
  {
    return hana::bool_c<false>;  
  }
  //todo return true for create and update that don't specify NoDiff

};

constexpr auto make_access_point = Make<AccessPoint>{};

}//builder
}//nbdl_def
#endif
