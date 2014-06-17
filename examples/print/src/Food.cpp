#include "Food.h"

void Food::validate()
{
	validateString("name")
		.required()
		.max(50);
}

void Food::bindMembers(Entity::Binder &b)
{
	bind(b, "foodGroup", foodGroup);
}
