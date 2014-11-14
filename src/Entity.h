#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>

#include "ValidatorNumber.h"
#include "ValidatorString.h"
#include "Binder.h"

class BackEnd;
class RelationMap;
class ErrorBinder;

class Entity
{
	public:

	virtual ~Entity() {}

	bool save(BackEnd &, ErrorBinder&);
	template<typename T>
	bool isDirty(T &field);
	bool isDirty();
	bool isNew();

	virtual void bindMembers(Binder &) {}
	void applyDiff(Binder &);

	virtual void validate(ErrorBinder &errors) = 0;

	protected:

	template<typename T>
	ValidatorNumber<T> validateNumber(ErrorBinder &e, T &field);
	ValidatorString validateString(ErrorBinder &e, std::string &field);

	template<typename T>
	void bind(Binder &b, const std::string name, T &field);
	template<typename T>
	void bindReadOnly(Binder &b, const std::string name, T &field);
	void bindPathKey(Binder &, const std::string, unsigned int &);


	friend class Binder; //for external use of set method

	template<typename T>
	void set(T value, T &field);

	private:

	class Diff
	{
		public:

		bool isDirty();

		template<typename TYPE>
		bool isDirty(TYPE &field, Entity *container);

		template<typename TYPE>
		void set(TYPE value, TYPE& field, Entity *container);

		private:

		std::vector<unsigned int> dirtyFields;
	};
	Diff diff;

};
#include "Entity.hpp"

//Validator implementations of convenience methods
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
