#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <Entity.hpp>
#include "FoodGroup.h"

class Food : public Entity
{
	public:

		//Food &setName(const std::string &v) { set("Name", v); return *this; }

		void bindMembers(Binder &);
		void validate(ErrorBinder &);

	private:

		unsigned id;
		std::string name;

		FoodGroup foodGroup;
};

#endif
