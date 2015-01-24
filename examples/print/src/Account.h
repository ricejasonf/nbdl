#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <EntityTraits.hpp>
//#include <Entity.hpp>
//#include "Address.h"

struct Account
{
	std::string nameFirst;
	std::string nameLast;
	std::string phoneNumber;
	//Address address;
	unsigned int age;

	//friend class nbdl::Entity<Account>;

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

	/*
	template<class Binder>
	void members(Binder &b)
	{
		member(b, "nameFirst", nameFirst);
		member(b, "nameLast", nameLast);
		member(b, "phoneNumber", phoneNumber);
		member(b, "age", age);
		member(b, "address", address);
	}
	*/

};
namespace nbdl {
template<>
struct EntityTraits<Account>
{
	using Members = MemberSet<
		NBDL_MEMBER(&Account::nameFirst),
		NBDL_MEMBER(&Account::nameLast),
		NBDL_MEMBER(&Account::phoneNumber),
		NBDL_MEMBER(&Account::age) >;
};
NBDL_MEMBER_NAME(Account, nameFirst);
NBDL_MEMBER_NAME(Account, nameLast);
NBDL_MEMBER_NAME(Account, phoneNumber);
NBDL_MEMBER_NAME(Account, age);
}//nbdl

#endif
