#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorString : public Validator<ValidatorString, std::string>
{
	public:

	ValidatorString(Entity &entity, std::string &field, ErrorBinder &e) : 
		Validator<ValidatorString, std::string>(entity, field, e) {}
	inline ValidatorString &minLen(const int);
	inline ValidatorString &maxLen(const int);
	inline ValidatorString &asEmail();
	inline ValidatorString &allDigits();
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid");

	inline bool isBlank();
};
#include "ValidatorString.hpp"

#endif
