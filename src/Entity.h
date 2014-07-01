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
	inline bool isDirty(T &field);
	inline bool isDirty();
	bool isNew(); //todo make this function be based off PathNode

	virtual void bindMembers(Binder &) {}

	//virtual RelationMap getRelationMap() = 0;

	virtual void validate(ErrorBinder &errors) = 0;

	protected:

	template<typename T>
	inline ValidatorNumber<T> validateNumber(ErrorBinder &e, T &field);
	inline ValidatorString validateString(ErrorBinder &e, std::string &field);

	template<typename T>
	inline void bind(Binder &b, const std::string name, T &field);

	friend class Binder; //for external use of set

	template<typename T>
	inline void set(T value, T &field);

	private:

	class Diff
	{
		public:

		inline bool isDirty();

		template<typename TYPE>
		inline bool isDirty(TYPE &field, Entity *container);

		template<typename TYPE>
		inline void set(TYPE value, TYPE& field, Entity *container);

		private:

		std::vector<unsigned int> dirtyFields;
	};
	Diff diff;

};
#include "Entity.hpp"
#include "Validator.hpp"

#endif
