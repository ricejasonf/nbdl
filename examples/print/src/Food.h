#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <EntityTraits.hpp>
#include "FoodGroup.h"

struct Food
{
	unsigned id;
	std::string name;
	FoodGroup foodGroup;
};
NBDL_ENTITY(Food,
	id,
	name,
	foodGroup );

#endif
