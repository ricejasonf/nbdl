#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <Entity.hpp>
#include "Address.h"

class Account : public Entity<Account>
{
	std::string nameFirst;
	std::string nameLast;
	std::string phoneNumber;
	Address address;
	unsigned int age;

	friend class Entity<Account>;

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

	template<class Binder>
	void members(Binder &b)
	{
		member(b, "nameFirst", nameFirst);
		member(b, "nameLast", nameLast);
		member(b, "phoneNumber", phoneNumber);
		member(b, "age", age);
		member(b, "address", address);
	}

};

#endif
