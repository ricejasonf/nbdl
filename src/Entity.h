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
	virtual bool isNew() = 0;

	virtual void bindMembers(Binder &) {}
	inline void applyDiff(Binder &);

	//virtual RelationMap getRelationMap() = 0;

	virtual void validate(ErrorBinder &errors) = 0;

	protected:

	template<typename T>
	inline ValidatorNumber<T> validateNumber(ErrorBinder &e, T &field);
	inline ValidatorString validateString(ErrorBinder &e, std::string &field);

	template<typename T>
	inline void bind(Binder &b, const std::string name, T &field);
	template<typename T>
	inline void bindReadOnly(Binder &b, const std::string name, T &field);
	inline void bindPathKey(Binder &, const std::string, unsigned int &);


	friend class Binder; //for external use of set method

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
