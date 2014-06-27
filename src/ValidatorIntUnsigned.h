#ifndef VALIDATORINTUNSIGNED_H
#define VALIDATORINTUNSIGNED_H

#include "Validator.h"

class ValidatorIntUnsigned : public ValidatorNumber<unsigned int>
{
	public:

	ValidatorIntUnsigned(Entity &entity, unsigned int &field) :
		ValidatorNumber<unsigned int>(entity, field) {}
};

#endif
