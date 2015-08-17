#ifndef NBDL_ENTITY_KEY_HPP
#define NBDL_ENTITY_KEY_HPP

#include "Member.hpp"

namespace nbdl {

template<typename EntityType>
struct GetPrimaryKey
{
	//default primary key is .id
	using Member = NBDL_MEMBER(&EntityType::id);
};

template<typename EntityType>
using PrimaryKey = typename GetPrimaryKey<EntityType>::Member;

template<typename EntityType, typename ForeignEntityType>
struct GetForeignKey {};

template<typename EntityType, typename ForeignEntityType>
using ForeignKey = typename GetForeignKey<EntityType, ForeignEntityType>::Member;

}//nbdl

#endif
