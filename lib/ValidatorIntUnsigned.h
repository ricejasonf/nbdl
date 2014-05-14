#ifndef VALIDATORSTRING_H
#define VALIDATORSTRING_H

#include "Validator.h"

class ValidatorIntUnsigned : public Validator
{
	public:

	ValidatorIntUnsigned(Entity &entity, const string name) :
		Entity(entity, name)
	{
		if (hasValue())
			_matches("^[0-9]+$", "notUnsigned");
	}

	ValidatorIntUnsigned &required() { _required(); return *this; }
	ValidatorIntUnsigned &optional() { _optional(); return *this; }
}

#endif
