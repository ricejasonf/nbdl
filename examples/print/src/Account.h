#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <EntityTraits.hpp>
#include "Address.h"
#include "Food.h"

struct Account
{
	std::string nameFirst;
	std::string nameLast;
	std::string phoneNumber;
	unsigned int age;
	Address address;
	Food food;

};
NBDL_ENTITY(
	Account,
		nameFirst,
		nameLast,
		phoneNumber,
		age,
		address,
		food);

namespace nbdl {
	NBDL_MEMBER_DEFAULT(&Account::phoneNumber, "7608675309");
	NBDL_MEMBER_MAXLENGTH(&Account::phoneNumber, 5000);

	template<class ValidationContext>
	void validate(ValidationContext &v, Account)
	{
		validateString<NBDL_MEMBER(&Account::nameFirst)>(v)
			.maxLen(50);
		validateString<NBDL_MEMBER(&Account::nameLast)>(v)
			.maxLen(50)
			.minLen(2);
		validateString<NBDL_MEMBER(&Account::phoneNumber)>(v)
			.matches("^[0-9]{10}$");
	}
	NBDL_MEMBER_MATCH(&Account::phoneNumber, "^[0-9]{10}$");
}//nbdl

#endif
