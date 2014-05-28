#include "Account.h"

void Account::validate()
{
	validateString("NameFirst")
		.required()
		.max(50);
	validateString("NameLast")
		.required()
		.max(50);
	/*
	validateString("PhoneNumber")
		.optional()
		.matches("^[0-9]{10}$");
		*/
}
