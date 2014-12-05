#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "ValidationBinder.hpp"
class Entity;

template<class Derived, typename T>
class Validator
{
	ValidationBinder &validationBinder;
	Entity &entity;
	bool chainBroken_;

	public:

	Validator(Entity &entity, T &field, ValidationBinder &e) :
		entity(entity),
		field(field),
		validationBinder(e),
		chainBroken_(false) {}

	virtual ~Validator() {}

	bool isUpdate()
	{
		return validationBinder.isUpdate();
	}

	void addError(const std::string error)
	{ 
		validationBinder.addError(entity, field, error);
		chainBroken_ = true; 
	}

	Derived &inSet(const std::vector<T> set)
	{
		if (!chainBroken_ && std::find(set.begin(), set.end(), field) == set.end())
			addError("notInSet");
		return static_cast<Derived&>(*this);
	}

	Derived &inSet(const std::unordered_set<T> set)
	{
		if (!chainBroken_ && set.find(field) != set.end())
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

	T &field;
	bool isChainBroken()
	{ 
		return chainBroken_;
	}
};

#endif
