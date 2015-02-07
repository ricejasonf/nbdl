#ifndef FOODGROUP_H
#define FOODGROUP_H

#include <string>
#include <EntityTraits.hpp>

struct FoodGroup 
{
	std::string name;
};
namespace nbdl {
	NBDL_ENTITY(FoodGroup, name);
}//nbdl

#endif
