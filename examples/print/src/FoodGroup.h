#ifndef FOODGROUP_H
#define FOODGROUP_H

#include <string>
#include <Entity.h>

class FoodGroup : public Entity
{
	public:
		FoodGroup(BackEnd::Ptr backEnd) : Entity(backEnd) {}

		FoodGroup &setName(const std::string &v) { set("Name", v); return *this; }

		void validate();
};

#endif
