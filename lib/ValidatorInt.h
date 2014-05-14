#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorInt : public Validator
{
	public:

	ValidatorInt(Entity &entity, const string name) :
		Entity(entity, name)
	{
		if (hasValue())
			_matches("^-?[0-9]+$", "notInt");
	}

	inline ValidatorInt &required() { _required(); return *this; }
	inline ValidatorInt &optional() { _optional(); return *this; }

	inline ValidateInt &inSet(vector<int> set) 
	{
		if (!chainBroken && set.find(set.begin(), set.end(), value) == set.end())
			addError("notInSet");
		return *this;
	}

	//todo range(int, int)
}

#endif
