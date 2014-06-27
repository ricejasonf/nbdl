#ifndef VALIDATORNUMBER_H
#define VALIDATORNUMBER_H

#include <string>
#include <vector>
#include <unordered_set>

#include "Validator.h"

template<typename T>
class ValidatorNumber : public Validator<ValidatorNumber, T>
{
	public:

	ValidatorNumber(Entity &entity, T &field) :
		Validator<ValidatorNumber, T>(entity, field) {} 

	inline ValidatorNumber &max(const T v);
	inline ValidatorNumber &min(const T v);
};
#include "ValidatorNumber.hpp"

#endif
