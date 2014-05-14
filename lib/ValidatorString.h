#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorString : public Validator
{
	public:

	ValidatorString(Entity &entity, const string name) : Validator(entity, name) {}
	inline ValidatorString &required() { _required(); return *this; }
	inline ValidatorString &optional() { _optional(); return *this; }
	inline ValidatorString &min(const int m) 
	{ 
		if (!chainBroken && value.size() < m) 
			addError("tooShort"); 
		return *this; 
	}
	inline ValidatorString &max(const int m) 
	{ 
		if (!chainBroken && value.size() > m) 
			addError("tooLong"); 
		return *this; 
	}
	inline ValidatorString &matches(const regex r const string token = "invalid") 
	{ 
		_matches(regex, token);
		return *this ;
	}
	inline ValidatorString &asEmail() 
	{ 
		if (!chainBroken)
			_matches("/^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$/"); 
		return *this; 
	}
	inline ValidatorString &allDigits();
	{ 
		if (!chainBroken)
			_matches("/^[0-9]*$/"); 
		return *this; 
	}
	inline ValidateString &inSet(vector<string> set) 
	{
		if (!chainBroken && set.find(value) == set.end())
			addError("notInSet");
		return *this;
	}
}

#endif
