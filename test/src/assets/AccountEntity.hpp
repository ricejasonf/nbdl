//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_ASSETS_ACCOUNT_HPP
#define NBDL_TEST_ASSETS_ACCOUNT_HPP

#include <string>
#include <EntityTraits.hpp>
#include <MemberSet.hpp>
#include <Bind.hpp>
struct Address
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zipCode;
};
namespace nbdl {
	NBDL_ENTITY(
		Address,
			line1,
			line2,
			city,
			state,
			zipCode );
}//nbdl

struct FoodGroup 
{
	std::string name;
};
namespace nbdl {
	NBDL_ENTITY(FoodGroup, name);
}//nbdl

struct Food
{
	unsigned id;
	std::string name;
	FoodGroup foodGroup;
};
namespace nbdl {
	NBDL_ENTITY(Food,
		id,
		name,
		foodGroup );
}//nbdl

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

static const char *account_entity_json_test_string = " { \"nameFirst\": \"Jason\", \"nameLast\": \"Rice\", \"phoneNumber\": \"7024569874\", \"email\": \"ricejasonf@gmail.com\", \"age\": 34, \"address\": { \"line1\":\"  123 Spork Rd.  \", \"line2\":\"\", \"city\":\"Las Vegas\", \"zipCode\":\"89015\", \"state\":\"NV\" }, \"food\": { \"id\": 1, \"name\": \"Banana\", \"foodGroup\": { \"name\": \"Fruits\" } } }";

#endif
