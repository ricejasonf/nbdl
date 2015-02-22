#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
#include "Address.h"
#include "Food.h"

struct Account
{
	std::string nameFirst;
	std::string nameLast;
	std::string phoneNumber;
	std::string email;
	unsigned int age;
	Address address;
	Food food;

};
namespace nbdl {
	NBDL_ENTITY(
		Account,
			nameFirst,
			nameLast,
			phoneNumber,
			email,
			age,
			address,
			food );

	NBDL_MEMBER_MINLENGTH(&Account::nameFirst, 2);
	NBDL_MEMBER_MINLENGTH(&Account::nameLast, 2);
	NBDL_MEMBER_MATCH(&Account::phoneNumber, "^[0-9]{10}$");
	NBDL_MEMBER_DEFAULT(&Account::phoneNumber, "7608675309");

	//NBDL_MEMBER_MAXLENGTH(&Account::phoneNumber, 5000);
	/*
	template<> 
	struct MemberCustomValidator<NBDL_MEMBER(&Account::email)>
	{
		template<typename AddError>
		static void validate(const std::string &value, AddError addError)
		{
			addError(ErrorToken::Custom1);
		}
	};
	*/
}//nbdl

#endif
