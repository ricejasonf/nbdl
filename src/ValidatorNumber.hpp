#ifndef VALIDATORNUMBER_H
#define VALIDATORNUMBER_H

#include <string>
#include <vector>
#include <unordered_set>

#include "Validator.hpp"

class Entity;

template<typename T>
class ValidatorNumber : public Validator<ValidatorNumber<T>, T>
{
	public:

	ValidatorNumber(Entity &entity, T &field, ValidationBinder &e) :
		Validator<ValidatorNumber, T>(entity, field, e) {} 

	inline ValidatorNumber &max(const T v);
	inline ValidatorNumber &min(const T v);
};

/*
 * IMPL
 */
template<typename T>
inline ValidatorNumber<T> &
	ValidatorNumber<T>::max(const T v)
{
	if (!this->isChainBroken() && this->field > v) 
		this->addError("tooBig");
	return *this;
}

template<typename T>
inline ValidatorNumber<T> &
	ValidatorNumber<T>::min(const T v)
{
	if (!this->isChainBroken() && this->field < v) 
		this->addError("tooSmall");
	return *this;
}

#endif
