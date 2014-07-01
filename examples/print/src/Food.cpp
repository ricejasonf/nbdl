#include "Food.h"

void Food::validate(ErrorBinder &e)
{
	validateString(e, "name")
		.required()
		.max(50);
}

void Food::bindMembers(Entity::Binder &b)
{
	bind(b, "foodGroup", foodGroup);
}
