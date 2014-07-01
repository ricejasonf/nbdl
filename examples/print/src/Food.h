#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <Entity.h>
#include "FoodGroup.h"

class Food : public Entity
{
	public:

		//Food &setName(const std::string &v) { set("Name", v); return *this; }

		void bindMembers(Binder &);
		void validate(ErrorBinder &);

	private:

		std::string name;

		FoodGroup foodGroup;
};

#endif
