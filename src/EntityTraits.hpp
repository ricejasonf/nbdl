#ifndef NBDL_ENTITYTRAITS_HPP
#define NBDL_ENTITYTRAITS_HPP

#include "MemberSet.hpp"

namespace nbdl {

template<typename Impl>
struct EntityTraits;

template<typename NameFormat, typename Entity, typename Binder>
void bind(Binder &binder, Entity &entity)
{
	EntityTraits<Entity>::Members::template bindMembers<NameFormat>(binder, entity);
}

}//nbdl
#endif
