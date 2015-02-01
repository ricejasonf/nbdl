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
	template<>
	struct EntityValidator<NBDL_MEMBER(&Account::nameFirst)>
	{
		using Validator = ValidatorString<
			Required,
			MaxLength<50>,
			Matches ??
				
	};




	template<class ValidationContext>
	void validate(ValidationContext &v, Account)
	{
		validateString<NBDL_MEMBER(&Account::nameFirst)>(v)
			.required()
			.maxLen(50)
			.minLen(2);
		validateString<NBDL_MEMBER(&Account::nameLast)>(v)
			.required()
			.maxLen(50)
			.minLen(2);
		validateString<NBDL_MEMBER(&Account::phoneNumber)>(v)
			.optional()
			.matches("^[0-9]{10}$");
	}
}//nbdl

#endif
