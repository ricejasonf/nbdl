#ifndef VALIDATORINTUNSIGNED_H
#define VALIDATORINTUNSIGNED_H

#include "Validator.h"

class ValidatorIntUnsigned : public Validator
{
	public:

	ValidatorIntUnsigned(Entity &entity, const std::string &name) :
		Validator(entity, name)
	{
		if (hasValue())
			_matches("^[0-9]+$", "notUnsigned");
	}

	ValidatorIntUnsigned &required() { _required(); return *this; }
	ValidatorIntUnsigned &optional() { _optional(); return *this; }
};

#endif
