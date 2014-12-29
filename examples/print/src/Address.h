#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <Entity.hpp>

class Address : public Entity<Address>
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zipCode;

	friend class Entity<Address>;

	template<class ValidationBinder>
	void validate(ValidationBinder &e)
	{
		validateString(e, line1)
			.required()
			.maxLen(50);
		validateString(e, line2)
			.optional()
			.maxLen(50);
		validateString(e, zipCode)
			.required()
			.matches("^[0-9]{5}$");
		validateString(e, city)
			.required();
		validateString(e, state)
			.required()
			.matches("^[A-Z]{2}$");
	}

	template<class Binder>
	void members(Binder &b)
	{
		member(b, "line1", line1);
		member(b, "line2", line2);
		member(b, "city", city);
		member(b, "state", state);
		member(b, "zipCode", zipCode);
	}
};

#endif
