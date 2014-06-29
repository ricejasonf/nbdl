#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>

#include "ValidatorNumber.h"
#include "ValidatorString.h"
#include "Binder.h"

class Entity
{
	public:

	virtual ~Entity();

	bool save();
	template<typename T>
	inline bool isDirty(T &field);
	inline bool isDirty();
	bool isNew(); //todo make this function be based of PathNode

	virtual void bindMembers(Binder &) {}

	virtual RelationMap getRelationMap = 0;

	friend class Validator;

	protected:

	virtual bool validate(ErrorBinder &errors) = 0;

	template<typename T>
	inline void set(T value, T &field);

	const std::string get(const std::string &name);
	void addError(const std::string &name, const std::string &error);

	template<typename T>
	inline ValidatorNumber &validateNumber(T &field, Errors &e);
	inline ValidatorString &validateString(std::string &field, Errors &e);
	template<typename T, class V>
	inline V &validate(T &field, ErrorBinder &e);

	template<type T>
	inline void bind(Binder &b, const std::string name, T &field);

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
	}
	Diff diff;

};
#include "Entity.hpp"

#endif
