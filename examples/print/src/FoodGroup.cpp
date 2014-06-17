#include "FoodGroup.h"

void FoodGroup::validate()
{
	validateString("name")
		.required()
		.max(50);
}
