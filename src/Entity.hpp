#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "ValidatorNumber.hpp"
#include "ValidatorString.hpp"
#include "Binder.h"

class ErrorBinder;

class Entity
{
	public:

	virtual ~Entity() {}

	template<typename T>
	bool isDirty(T &field)
	{
		return diff.isDirty(field, this);
	}

	bool isDirty()
	{
		return diff.isDirty();
	}

	bool isNew()
	{
		return true;
	}

	virtual void bindMembers(Binder &) {}

	void applyDiff(Binder &binder)
	{
		binder.setDiffMode(true);	
		bindMembers(binder);
	}

	virtual void validate(ErrorBinder &errors) = 0;

	protected:

	template<typename T>
	ValidatorNumber<T> validateNumber(ErrorBinder &e, T &field)
	{ 
		return ValidatorNumber<T>(*this, field, e);
	}

	ValidatorString validateString(ErrorBinder &e, std::string &field)
	{ 
		return ValidatorString(*this, field, e);
	}

	template<typename T>
	void bind(Binder &b, const std::string name, T &field)
	{ 
		b.bind(*this, name, field); 
	}

	template<typename T>
	void bindReadOnly(Binder &b, const std::string name, T &field)
	{
		if (b.diffMode()) 
			return;
		bind(b, name, field);
	}
	void bindPathKey(Binder &b, const std::string name, unsigned int &field)
	{
		//prevents modifying a key that is already established
		//this should be used for every key in the path
		if (b.diffMode() && field > 0) 
			return;
		bind(b, name, field);
	}

	friend class Binder; //for external use of set method

	template<typename T>
	void set(T value, T &field)
	{
		diff.set(value, field, this);
	}

	private:

	class Diff
	{
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

		private:

		std::vector<unsigned int> dirtyFields;
	};
	Diff diff;

};

//Binder implementation
template<typename T>
inline void 
Binder::set(Entity &e, T value, T &field) { e.set(value, field); }

//Validator implementation of convenience methods
template<class Derived, typename T>
Derived& Validator<Derived, T>::required()
{
	if (entity.isNew())
	{
		bool hasValue = entity.isDirty(field);
		if (hasValue && isBlank())
			addError("required");	
		else if (!hasValue)
			chain_broken = true;
	}
	else if (isBlank())
		addError("required");
}

template<class Derived, typename T>
Derived& Validator<Derived, T>::optional() 
{ 
	if (entity.isDirty(field)) 
		chain_broken = true; 
}

#endif
