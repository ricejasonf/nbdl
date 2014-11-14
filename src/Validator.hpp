#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "ErrorBinder.h"
class Entity;

template<class Derived, typename T>
class Validator
{
	public:

	Validator(Entity &entity, T &field, ErrorBinder &e) :
		entity(entity),
		field(field),
		errorBinder(e),
		chain_broken(false) {}

	virtual ~Validator() {}

	void addError(const std::string error)
	{ 
		errorBinder.addError(entity, field, error);
		chain_broken = true; 
	}

	Derived &inSet(const std::vector<T> set)
	{
		if (!chain_broken && std::find(set.begin(), set.end(), field) == set.end())
			addError("notInSet");
		return static_cast<Derived&>(*this);
	}
	Derived &inSet(const std::unordered_set<T> set)
	{
		if (!chain_broken && set.find(field) != set.end())
			addError("notInSet");
		return static_cast<Derived&>(*this);
	}

	virtual bool isBlank()
	{ 
		return false; 
	}

	//the following are implemented in Entity.hpp
	Derived &required();
	Derived &optional();

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
