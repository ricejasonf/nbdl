#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "ValidatorNumber.hpp"
#include "ValidatorString.hpp"
#include "Binder.hpp"

class ValidationBinder;

class Entity
{
	virtual void members(Binder &) = 0;
	virtual void validate(ValidationBinder &) = 0;

	class Diff
	{
		std::vector<unsigned int> dirtyFields;

		public:

		bool isDirty()
		{
			return dirtyFields.size() > 0;
		}

		template<typename TYPE>
		bool isDirty(TYPE &field, Entity *container)
		{
			return std::find(
				dirtyFields.begin(),
				dirtyFields.end(),
				(uintptr_t)&field - (uintptr_t)container) != dirtyFields.end();
		}

		template<typename TYPE>
		void set(TYPE value, TYPE& field, Entity *container)
		{
			field = value;
			dirtyFields.push_back((uintptr_t)&field - (uintptr_t)container);
		}
	};
	Diff diff;

	public:

	virtual ~Entity() {}

	void runValidation(ValidationBinder &binder)
	{
		validate(binder);
	}

	void bindMembers(Binder &binder)
	{
		members(binder);
	}

	template<typename T>
	bool isDirty(T &field)
	{
		return diff.isDirty(field, this);
	}

	bool isDirty()
	{
		return diff.isDirty();
	}

	//todo deprecate
	void applyDiff(Binder &binder)
	{
		binder.setDiffMode(true);	
		bindMembers(binder);
	}

	//for use by binders
	template<typename T>
	void set(T value, T &field)
	{
		diff.set(value, field, this);
	}

	protected:

	template<typename T>
	ValidatorNumber<T> validateNumber(ValidationBinder &e, T &field)
	{ 
		return ValidatorNumber<T>(*this, field, e);
	}

	ValidatorString validateString(ValidationBinder &e, std::string &field)
	{ 
		return ValidatorString(*this, field, e);
	}

	template<typename T>
	void member(Binder &b, const std::string name, T &field)
	{ 
		b.bindMember(*this, name, field); 
	}

	//not used yet
	template<typename T>
	void memberReadOnly(Binder &b, const std::string name, T &field)
	{
		if (b.diffMode()) 
			return;
		member(b, name, field);
	}
	//not used yet
	void memberPathKey(Binder &b, const std::string name, unsigned int &field)
	{
		//prevents modifying a key that is already established
		//this should be used for every key in the path
		if (b.diffMode() && field > 0) 
			return;
		member(b, name, field);
	}
};

//Binder implementation
template<typename T>
inline void 
Binder::set(Entity &e, T value, T &field) { e.set(value, field); }

//Validator implementation of convenience methods
template<class Derived, typename T>
Derived& Validator<Derived, T>::required()
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
	return static_cast<Derived &>(*this);
}

template<class Derived, typename T>
Derived& Validator<Derived, T>::optional() 
{ 
	if (!entity.isDirty(field)) 
		chainBroken_ = true; 
	return static_cast<Derived &>(*this);
}

#endif
