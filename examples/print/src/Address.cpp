#include "Address.h"

void Address::validate()
{
	validateString("line1")
		.required()
		.max(50);
	validateString("line2")
		.optional()
		.max(50);
	validateString("zipCode")
		.required()
		.matches("^[0-9]{5}$");
	validateString("city")
		.required();
	validateString("state")
		.required()
		.matches("^[A-Z]{2}$");
}
