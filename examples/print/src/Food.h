#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <Entity.h>
#include "FoodGroup.h"

class Food : public Entity
{
	public:
		Food(BackEnd::Ptr backEnd) : 
			foodGroup(FoodGroup(backEnd)),
			Entity(backEnd) {}

		Food &setName(const std::string &v) { set("Name", v); return *this; }

		void bindMembers(Binder &);
		void validate();

	private:

		FoodGroup foodGroup;
};

#endif
