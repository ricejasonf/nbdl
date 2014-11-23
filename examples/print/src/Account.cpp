#include "Account.h"

void Account::validate(ErrorBinder &e)
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

void Account::bindMembers(Binder &b)
{
	bind(b, "nameFirst", nameFirst);
	bind(b, "nameLast", nameLast);
	bind(b, "phoneNumber", phoneNumber);
	bind(b, "age", age);
	bind(b, "address", address);
	bind(b, "foods", foods);
}

