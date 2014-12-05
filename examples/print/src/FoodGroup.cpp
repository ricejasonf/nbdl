#include "FoodGroup.h"

void FoodGroup::validate(ValidationBinder &e)
{
	validateString(e, name)
		.required()
		.maxLen(50);
}

void FoodGroup::members(Binder &b)
{
	member(b, "name", name);
}
