#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <memory>
#include "../../lib/Entity.h"

class Account : public Entity
{
	public:
		Account(std::unique_ptr<BackEnd> backEnd) : Entity(std::move(backEnd)) {}

		Account &setNameFirst(const std::string &v) { set("NameFirst", v); return *this; }
		Account &setNameLast(const std::string &v) { set("NameLast", v); return *this; }
		Account &setPhoneNumber(const std::string &v) { set("PhoneNumber", v); return *this; }
		Account &setUsername(const std::string &v) { set("Username", v); return *this; }
		Account &setPassword(const std::string &v) { set("Password", v); return *this; }
		Account &setAge(const std::string &v) { set("Age", v); return *this; }

		void validate();
};

#endif
