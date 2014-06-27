#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Entity;

template<class C, typename T>
class Validator
{
	public:

	Validator(Entity &entity, T &field) :
		entity(entity),
		field(field),
		chain_broken(false) {}

	virtual ~Validator() {}

	protected:

	inline C &addError(const std::string error);
	inline C &inSet(const std::vector<T> set);
	inline C &inSet(const std::unordered_set<T> set);
	inline C &required();
	inline C &optional();

	inline bool isBlank();

	bool chain_broken;

	Entity &entity;
	T &field;
};
#include "Validator.hpp"

#endif
