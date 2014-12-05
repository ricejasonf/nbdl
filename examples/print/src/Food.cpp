#include "Food.h"

void Food::validate(ValidationBinder &e)
{
	validateString(e, name)
		.required()
		.maxLen(50);
}

void Food::members(Binder &b)
{
	member(b, "id", id);
	member(b, "name", name);
	member(b, "foodGroup", foodGroup);
}
