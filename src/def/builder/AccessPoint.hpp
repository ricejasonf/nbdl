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

template<typename ContextBuilder_, typename AccessPointDef_>
class AccessPoint
{
  using ContextBuilder = ContextBuilder_;
  using AccessPointDef = AccessPointDef_;

  const ContextBuilder ctx;
  const AccessPointDef def;

  constexpr AccessPoint(ContextBuilder&& c, AccessPointDef&& a) :
    ctx(std::forward<ContextBuilder>(c)),
    def(std::forward<AccessPointDef>(a))
  {}

  constexpr auto pathType()
  {
    auto path_def = *meta::findByTag(*meta::findByTag(def, tag::Path), tag::Type);
    //todo actually use a path definition instead of embedding the type
    return hana::at(path_def, hana::int_c<0>);
  }

  constexpr auto entityType()
  {
    using Path_ = typename decltype(pathType())::type;
    using Entity_ = typename Path_::Entity;
    return hana::type_c<Entity_>;
  }

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
