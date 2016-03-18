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
#include <Bind.hpp>

struct address
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zip_code;
};
namespace nbdl {
	NBDL_ENTITY(
		address,
			line1,
			line2,
			city,
			state,
			zip_code );
}//nbdl

struct food_group 
{
	std::string name;
};
namespace nbdl {
	NBDL_ENTITY(food_group, name);
}//nbdl

struct food
{
	unsigned id;
	std::string name;
	food_group food_group;
};
namespace nbdl {
	NBDL_ENTITY(food,
		id,
		name,
		food_group );
}//nbdl

struct account
{
	std::string name_first;
	std::string name_last;
	std::string phone_number;
	std::string email;
	unsigned int age;
	address address;
	food food;

};
namespace nbdl {
	NBDL_ENTITY(
		account,
			name_first,
			name_last,
			phone_number,
			email,
			age,
			address,
			food );

	NBDL_MEMBER_MINLENGTH(&account::name_first, 2);
	NBDL_MEMBER_MINLENGTH(&account::name_last, 2);
	NBDL_MEMBER_MATCH(&account::phone_number, "^[0-9]{10}$");
	NBDL_MEMBER_DEFAULT(&account::phone_number, "7608675309");

	//NBDL_MEMBER_MAXLENGTH(&account::phone_number, 5000);
	/*
	template<> 
	struct Member_custom_validator<NBDL_MEMBER(&account::email)>
	{
		template<typename AddError>
		static void validate(const std::string &value, AddError addError)
		{
			addError(ErrorToken::Custom1);
		}
	};
	*/
}//nbdl

static const char *account_entity_json_test_string = " { \"name_first\": \"Jason\", \"name_last\": \"Rice\", \"phone_number\": \"7024569874\", \"email\": \"ricejasonf@gmail.com\", \"age\": 34, \"address\": { \"line1\":\"  123 Spork Rd.  \", \"line2\":\"\", \"city\":\"Las Vegas\", \"zip_code\":\"89015\", \"state\":\"NV\" }, \"food\": { \"id\": 1, \"name\": \"Banana\", \"food_group\": { \"name\": \"Fruits\" } } }";

#endif
