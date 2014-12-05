#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <Entity.hpp>
#include "FoodGroup.h"

class Food : public Entity
{
	void members(Binder &);
	void validate(ValidationBinder &);

	unsigned id;
	std::string name;

	FoodGroup foodGroup;
};

#endif
