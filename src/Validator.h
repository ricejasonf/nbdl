#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <vector>
#include <unordered_set>
#include "ErrorBinder.h"
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

	inline void addError(const std::string error);
	inline C &inSet(const std::vector<T> set);
	inline C &inSet(const std::unordered_set<T> set);
	inline C &required();
	inline C &optional();

	inline virtual bool isBlank();

	protected:

	bool chain_broken;

	Entity &entity;
	T &field;
	ErrorBinder &errorBinder;
};

/*
	call validate method to populate error container
	call bindMembers and bind the errors to the binder
*/

#endif
