//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ENTITY_HPP
#define NBDL_DEF_ENTITY_HPP

#include "../builder.hpp"
#include "./Path.hpp"

namespace nbdl_def {
namespace builder {

template<typename ContextBuilder, typename EntityDef>
struct Entity
{
  ContextBuilder ctx;
  EntityDef def;

  constexpr Entity(ContextBuilder c, EntityDef a) :
    ctx(c),
    access_point(a)
  {}

  template<typename AccessPoint>
  constexpr auto entityFromAccessPoint(AccessPoint access_point)
  {
    using Path_ = typename decltype(path(access_point))::type;
    using Entity_ = typename Path_::Entity;
    return hana::type_c<Entity_>;
  }

  template<typename EntityType>
  constexpr auto entityHasValidation(EntityType)
  {
    return hana::bool_c<false>;
  }

  template<typename EntityType>
  constexpr auto entityHasLocalVersion(EntityType)
  {
    return hana::bool_c<false>;  
    //todo stores will have special 'LocalVersion' objects if this is true
    //todo it also means create and update messages will have a uuid
  }
};
/*
 * TODO: Maybe replace the use of the following template specializations
 * that were used to define entities for serialization, validation,
 * and other stuff:
 *
 * EntityTraits
 * MemberId (integral identifier for serialization)
 * MemberName
 * Member* (pretty much all that member definition stuff)
 * GetPrimaryKey
 * GetForeignKey (relation stuff that can wait)
 *
 * note: I would abandon this if it were to increase the size
 * of the release target
 *
 */

}//builder
}//nbdl_def
#endif
