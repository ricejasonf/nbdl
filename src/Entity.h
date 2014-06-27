#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>

#include "ValidatorInt.h"
#include "ValidatorIntUnsigned.h"
#include "ValidatorString.h"
#include "BackEnd.h"
class EntityListBase;
typedef std::unordered_map<std::string, std::string> ValueMap;
typedef std::unordered_map<std::string, std::vector<std::string>> ErrorList;

class Entity
{
	public:

	Entity(Entity&) :
		validators(std::vector<std::unique_ptr<Validator>>()) {}
	Entity(Entity&&) :
		validators(std::vector<std::unique_ptr<Validator>>()) {}
	virtual ~Entity() { }

	bool save();

	virtual RelationMap getRelationMap = 0;

	friend class Validator;
	friend class EntityBuilder;
	friend class Binder;
	class Binder
	{
		protected:

		friend class Entity;
		virtual void bind(const std::string, bool &) = 0;
		virtual void bind(const std::string, unsigned int &) = 0;
		virtual void bind(const std::string, int &) = 0;
		virtual void bind(const std::string, double &) = 0;
		virtual void bind(const std::string, std::string &) = 0;
		virtual void bind(const std::string, Entity &) = 0;
		virtual void bind(const std::string, EntityListBase &) = 0;

		inline void bindMembers(Entity &e);
	};

	protected:

	virtual void bindMembers(Binder &) {}
	virtual ValueMap validate(ValueMap changes) = 0;

	template<typename T>
	void set(T value, T &field);

	const std::string get(const std::string &name);
	void addError(const std::string &name, const std::string &error);

	template<typename T>
	inline ValidatorNumber &validateNumber(T &field, Errors &e);
	inline ValidatorString &validateString(std::string &field, Errors &e);
	template<typename T, class V>
	inline V &validate(T &field, , Errors &e);

	template<type T>
	inline void bind(Binder &b, const std::string name, T &field);

	private:

	EntityDiff diff;

};
#include "Entity.hpp"

#endif
