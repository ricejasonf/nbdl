#include "Address.h"

void Address::validate(ValidationBinder &e)
{
	validateString(e, line1)
		.required()
		.maxLen(50);
	validateString(e, line2)
		.optional()
		.maxLen(50);
	validateString(e, zipCode)
		.required()
		.matches("^[0-9]{5}$");
	validateString(e, city)
		.required();
	validateString(e, state)
		.required()
		.matches("^[A-Z]{2}$");
}

void Address::members(Binder &b)
{
	member(b, "line1", line1);
	member(b, "line2", line2);
	member(b, "city", city);
	member(b, "state", state);
	member(b, "zipCode", zipCode);
}
