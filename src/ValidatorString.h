#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorString : public Validator<ValidatorString, std::string>
{
	public:

	ValidatorString(Entity &entity, const std::string &name) : 
		Validator<ValidatorString, std::string>(entity, name) {}
	inline ValidatorString &minLen(const int m);
	inline ValidatorString &maxLen(const int m);
	inline ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid");
	inline ValidatorString &asEmail();
	inline ValidatorString &allDigits();
};
#include "ValidatorString.hpp"

#endif
