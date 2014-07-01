#include "Food.h"

void Food::validate(ErrorBinder &e)
{
	validateString(e, name)
		.required()
		.maxLen(50);
}

void Food::bindMembers(Binder &b)
{
	bind(b, "id", id);
	bind(b, "name", name);
	bind(b, "foodGroup", foodGroup);
}
