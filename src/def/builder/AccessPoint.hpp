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

template<typename DefPath>
class AccessPoint
{
  using Tag = tag::AccessPoint;
  static constexpr allowed_parent_tags = hana::tuple_t<tag::AccessPoint, tag::Context>;
  static constexpr auto def = hana::first(hana::at_c<0>(DefPath{}));

  //todo make AccessPoints nestable and get rid of Path directive
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

}//builder
}//nbdl_def
#endif
