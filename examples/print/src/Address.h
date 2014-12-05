#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <Entity.hpp>

class Address : public Entity
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zipCode;

	void members(Binder &);
	void validate(ValidationBinder &);

	public:

	/*
	Address &setLine1(const std::string &v) { set("line1", v); return *this; }
	Address &setLine2(const std::string &v) { set("line2", v); return *this; }
	Address &setCity(const std::string &v) { set("city", v); return *this; }
	Address &setZipCode(const std::string &v) { set("zipCode", v); return *this; }
	*/

};

#endif
