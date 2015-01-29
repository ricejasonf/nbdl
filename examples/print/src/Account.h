#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <EntityTraits.hpp>
//#include <Entity.hpp>
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

	//todo move validate to diff class
	template<class ValidationBinder>
	void validate(ValidationBinder &b)
	{
		validateString(b, nameFirst)
			.required()
			.maxLen(50);
		validateString(b, nameLast)
			.required()
			.maxLen(50);
		validateString(b, phoneNumber)
			.optional()
			.matches("^[0-9]{10}$");
	}

};
NBDL_ENTITY(
	Account,
		nameFirst,
		nameLast,
		phoneNumber,
		age,
		address,
		food);

#endif
