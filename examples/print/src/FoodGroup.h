#ifndef FOODGROUP_H
#define FOODGROUP_H

#include <string>
#include <Entity.h>

class FoodGroup : public Entity
{
	std::string name;

	public:

		//FoodGroup &setName(const std::string v) { set(v, name); return *this; }

		void validate(ErrorBinder &);
};

#endif
