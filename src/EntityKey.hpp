//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY_KEY_HPP
#define NBDL_ENTITY_KEY_HPP
//this way of specifying key types will probably go away

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
