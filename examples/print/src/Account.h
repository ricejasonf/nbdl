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
	nbdl::optional<unsigned int> age;
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
	//unnecessary macro maybe?
	//NBDL_SPEC(member_default, &Account::phoneNumber)
	template<>
	struct member_default<NBDL_MEMBER(&Acccount::phoneNumber)>
	{
		static const std::string value = "7608675309";
	};
	//NBDL_SPEC(string_max_length, &Account::phoneNumber)
	template<>
	struct string_max_length<NBDL_MEMBER(&Acccount::phoneNumber)>
	{
		//default max for all strings should always be 50 or something
		static const int value = 50;
	};

	template<class ValidationContext>
	void validate(ValidationContext &v, Account)
	{
		validateString<NBDL_MEMBER(&Account::nameFirst)>(v)
			.notBlank()
			.maxLen(50);
		validateString<NBDL_MEMBER(&Account::nameLast)>(v)
			.notBlank()
			.maxLen(50)
			.minLen(2);
		validateString<NBDL_MEMBER(&Account::phoneNumber)>(v)
			.optional()
			.matches("^[0-9]{10}$");
	}
}//nbdl

#endif
