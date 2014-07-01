#include "FoodGroup.h"

void FoodGroup::validate(ErrorBinder &e)
{
	validateString(e, name)
		.required()
		.maxLen(50);
}

void FoodGroup::bindMembers(Binder &b)
{
	bind(b, "name", name);
}
