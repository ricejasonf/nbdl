#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorString : public Validator
{
	public:

	ValidatorString(Entity &entity, const std::string &name) : Validator(entity, name) {}
	inline ValidatorString &required() { _required(); return *this; }
	inline ValidatorString &optional() { _optional(); return *this; }
	inline ValidatorString &min(const int m) 
	{ 
		if (!chain_broken && value.size() < m) 
			addError("tooShort"); 
		return *this; 
	}
	inline ValidatorString &max(const int m) 
	{ 
		if (!chain_broken && value.size() > m) 
			addError("tooLong"); 
		return *this; 
	}
	inline ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid") 
	{ 
		_matches(reg, errorToken);
		return *this ;
	}
	inline ValidatorString &asEmail() 
	{ 
		if (!chain_broken)
			_matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
		return *this; 
	}
	inline ValidatorString &allDigits()
	{ 
		if (!chain_broken)
			_matches("^[0-9]*$"); 
		return *this; 
	}
	inline ValidatorString &inSet(std::vector<std::string> set)
	{
		if (!chain_broken && std::find(set.begin(), set.end(), value) == set.end())
			addError("notInSet");
		return *this;
	}
};

#endif
