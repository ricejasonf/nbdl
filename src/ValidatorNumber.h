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

	ValidatorNumber(Entity &entity, T &field, ErrorBinder &e) :
		Validator<ValidatorNumber, T>(entity, field, e) {} 

	inline ValidatorNumber &max(const T v);
	inline ValidatorNumber &min(const T v);
};
#include "ValidatorNumber.hpp"

#endif
