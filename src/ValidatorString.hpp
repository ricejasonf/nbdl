#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.hpp"

class ValidatorString : public Validator<ValidatorString, std::string>
{
	public:

	ValidatorString(Entity &entity, std::string &field, ErrorBinder &e) : 
		Validator<ValidatorString, std::string>(entity, field, e) {}
	ValidatorString &minLen(const int);
	ValidatorString &maxLen(const int);
	ValidatorString &asEmail();
	ValidatorString &allDigits();
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid");

	bool isBlank();
};

/*
 * IMPL
 */
ValidatorString &
	ValidatorString::minLen(const int l) 
{ 
	if (!chain_broken && field.size() < l) 
		addError("tooShort"); 
	return *this; 
}
ValidatorString &
	ValidatorString::maxLen(const int l) 
{ 
	if (!chain_broken && field.size() > l) 
		addError("tooLong"); 
	return *this; 
}
ValidatorString &
	ValidatorString::asEmail() 
{ 
	if (!chain_broken)
		matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
	return *this; 
}
ValidatorString &
	ValidatorString::allDigits()
{ 
	if (!chain_broken)
		matches("^[0-9]*$"); 
	return *this; 
}
bool 
	ValidatorString::isBlank() 
{ 
	return field.size() > 0;	
}

#endif
