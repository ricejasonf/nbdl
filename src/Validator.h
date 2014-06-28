#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Entity;

template<class C, typename T>
class Validator
{
	public:

	Validator(Entity &entity, T &field, ErrorBinder &e) :
		entity(entity),
		field(field),
		errorBinder(e),
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
	ErrorBinder &errorbinder;
};
#include "Validator.hpp"

/*
	call validate method to populate error container
	call bindMembers and bind the errors to the binder
*/

#endif
