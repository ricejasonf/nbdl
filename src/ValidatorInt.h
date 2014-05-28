#ifndef VALIDATORINT_H
#define VALIDATORINT_H

#include <algorithm>//break out into cpp file

#include "Validator.h"

class ValidatorInt : public Validator
{
	public:

	ValidatorInt(Entity &entity, const std::string &name) :
		Validator(entity, name)
	{
		if (hasValue())
			_matches("^-?[0-9]+$", "notInt");
	}

	inline ValidatorInt &required() { _required(); return *this; }
	inline ValidatorInt &optional() { _optional(); return *this; }

	/*
	inline ValidatorInt &inSet(std::vector<std::string> set) 
	{
		if (!chain_broken && std::find(set.begin(), set.end(), value) == set.end())
			addError("notInSet");
		return *this;
	}
	*/

	//todo range(int, int)
};

#endif
