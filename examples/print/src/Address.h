#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <Entity.h>

class Address : public Entity
{
	public:
		Address(BackEnd::Ptr backEnd) : Entity(backEnd) {}

		Address &setLine1(const std::string &v) { set("line1", v); return *this; }
		Address &setLine2(const std::string &v) { set("line2", v); return *this; }
		Address &setCity(const std::string &v) { set("city", v); return *this; }
		Address &setZipCode(const std::string &v) { set("zipCode", v); return *this; }

		void validate();
};

#endif
