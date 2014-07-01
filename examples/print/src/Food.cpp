#include "Food.h"

void Food::validate(ErrorBinder &e)
{
	validateString(e, name)
		.required()
		.maxLen(50);
}

void Food::bindMembers(Binder &b)
{
	bind(b, "foodGroup", foodGroup);
}
