#include "Account.h"

void Account::validate(ValidationBinder &e)
{
	validateString(e, nameFirst)
		.required()
		.maxLen(50);
	validateString(e, nameLast)
		.required()
		.maxLen(50);
	validateString(e, phoneNumber)
		.optional()
		.matches("^[0-9]{10}$");
}

void Account::members(Binder &b)
{
	member(b, "nameFirst", nameFirst);
	member(b, "nameLast", nameLast);
	member(b, "phoneNumber", phoneNumber);
	member(b, "age", age);
	member(b, "address", address);
	member(b, "foods", foods);
}
