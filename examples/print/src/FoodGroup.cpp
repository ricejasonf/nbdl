#include "FoodGroup.h"

void FoodGroup::validate(ErrorBinder &e)
{
	validateString(e, name)
		.required()
		.max(50);
}
