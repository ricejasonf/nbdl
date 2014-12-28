#ifndef VALIDATORNUMBER_H
#define VALIDATORNUMBER_H

#include <string>
#include <vector>
#include <unordered_set>

#include "Validator.hpp"

class Entity;

template<class ValidationBinder, class Entity, typename T>
class ValidatorNumber : public Validator<ValidatorNumber<ValidationBinder, EntityType, T>, T>
{
	public:

	typedef ValidationBinder ValidationBinderType;

	ValidatorNumber(Entity &entity, T &field, ValidationBinder &e) :
		Validator<ValidatorNumber, T>(entity, field, e) {} 

	ValidatorNumber &max(const T v)
	{
		if (!this->isChainBroken() && this->field > v) 
			this->addError("tooBig");
		return *this;
	}
	ValidatorNumber &min(const T v)
	{
		if (!this->isChainBroken() && this->field < v) 
			this->addError("tooSmall");
		return *this;
	}
};

//ValidatorTraits specialization
template<class ValidationBinderType, class EntityType, typename T>
struct ValidatorTraits<ValidatorNumber<ValidationBinderType, EntityType, T>>
{
	typedef ValidationBinderType ValidationBinder;
	typedef EntityType Entity;
};

#endif
