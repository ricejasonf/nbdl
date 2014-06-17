#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <Entity.h>
#include <EntityList.h>
#include "Address.h"
#include "Food.h"

class Account : public Entity
{
	public:
		Account(BackEnd::Ptr backEnd) : 
			address(Address(backEnd)), 
			foods(EntityList<Food>(backEnd)),
			Entity(backEnd) {}

		void bindMembers(Binder &b);
		Account &setNameFirst(const std::string &v) { set("nameFirst", v); return *this; }
		Account &setNameLast(const std::string &v) { set("nameLast", v); return *this; }
		Account &setPhoneNumber(const std::string &v) { set("phoneNumber", v); return *this; }
		Account &setUsername(const std::string &v) { set("username", v); return *this; }
		Account &setPassword(const std::string &v) { set("password", v); return *this; }
		Account &setAge(const std::string &v) { set("age", v); return *this; }

		void validate();

	private:

		Address address;
		EntityList<Food> foods;
};

#endif
