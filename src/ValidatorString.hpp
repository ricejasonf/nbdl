#ifndef VALIDATORSTRING_HPP
#define VALIDATORSTRING_HPP

#include "Validator.hpp"

class ValidatorString : public Validator<ValidatorString, std::string>
{
	public:

	ValidatorString(Entity &entity, std::string &field, ValidationBinder &e) : 
		Validator<ValidatorString, std::string>(entity, field, e) {}
	ValidatorString &minLen(const int);
	ValidatorString &maxLen(const int);
	ValidatorString &asEmail();
	ValidatorString &allDigits();
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid");

	bool isBlank();
};

#endif
