#ifndef NBDL_ENTITYTRAITS_HPP
#define NBDL_ENTITYTRAITS_HPP

namespace nbdl {

template<typename Impl>
struct EntityTraits;

template<typename NameFormat, typename Binder, typename Entity>
void bind(Binder &binder, Entity &entity)
{
	EntityTraits<Entity>::Members::template bindMembers<NameFormat>(binder, entity);
}
#include "MemberSet.hpp"

}//nbdl
#endif
