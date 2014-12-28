#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

template<class Impl>
struct ValidatorTraits;

template<class Impl, typename T>
class Validator
{
	typedef typename ValidatorTraits<Impl>::ValidationBinder ValidationBinder;
	typedef typename ValidatorTraits<Impl>::Entity Entity;
	ValidationBinder &validationBinder;
	Entity &entity;

	bool chainBroken_;

	public:

	Validator(EntityType &entity, T &field, ValidationBinderType &e) :
		entity(entity),
		field(field),
		validationBinder(e),
		chainBroken_(false) {}

	bool isUpdate()
	{
		return validationBinder.isUpdate();
	}

	void addError(const std::string error)
	{ 
		validationBinder.addError(entity, field, error);
		chainBroken_ = true; 
	}

	Impl &inSet(const std::vector<T> set)
	{
		if (!chainBroken_ && std::find(set.begin(), set.end(), field) == set.end())
			addError("notInSet");
		return static_cast<Impl&>(*this);
	}

	Impl &inSet(const std::unordered_set<T> set)
	{
		if (!chainBroken_ && set.find(field) != set.end())
			addError("notInSet");
		return static_cast<Impl&>(*this);
	}

	Impl& required()
	{
		if (!isUpdate())
		{
			bool hasValue = entity.isDirty(field);
			if (hasValue && isBlank())
				addError("required");	
			else if (!hasValue)
				chainBroken_ = true;
		}
		else if (isBlank())
			addError("required");
		return static_cast<Impl &>(*this);
	}

	Impl& optional() 
	{ 
		if (!entity.isDirty(field)) 
			chainBroken_ = true; 
		return static_cast<Impl &>(*this);
	}

	protected:

	bool isBlank()
	{ 
		return static_cast<Impl*>(this)->isBlank();
	}

	T &field;
	bool isChainBroken()
	{ 
		return chainBroken_;
	}
};

#endif
