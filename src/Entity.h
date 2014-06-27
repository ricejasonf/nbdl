#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

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

	int getId() { return id; }
	bool save();

	const ErrorList getErrors() { return errors; }

	virtual RelationMap getRelationMap = 0;

	friend class Validator;
	friend class EntityBuilder;
	friend class Binder;
	class Binder
	{
		protected:

		friend class Entity;
		virtual void bind(const std::string, &bool) = 0;
		virtual void bind(const std::string, &unsigned int) = 0;
		virtual void bind(const std::string, &int) = 0;
		virtual void bind(const std::string, &double) = 0;
		virtual void bind(const std::string, &std::string) = 0;
		virtual void bind(const std::string, Entity &) = 0;
		virtual void bind(const std::string, EntityListBase &) = 0;

		inline void bindMembers(Entity &e) { e.bindMembers(*this); };
		inline void initValue(Entity &entity, const std::string name, const std::string value) { entity.initValue(name, value); }
	};

	protected:

	virtual void bindMembers(Binder &) {}

	template<typename T>
	void set(T value, T &field)
	{
		diff.set(value, field, this);
	}
	const std::string get(const std::string &name);
	void addError(const std::string &name, const std::string &error);

	inline ValidatorIntUnsigned &validateIntUnsigned(const std::string name) { return validate<ValidatorIntUnsigned>(name); }
	inline ValidatorInt &validateInt(const std::string name) { return validate<ValidatorInt>(name); }
	inline ValidatorString &validateString(const std::string name) { return validate<ValidatorString>(name); }

	/*
	inline void bind(Binder &b, const std::string name, int &v) { b.bind(name, v); }
	//todo implement bind functions for all supported types
	inline void bind(Binder &b, const std::string name, Entity &entity) { b.bind(name, entity); }
	inline void bind(Binder &b, const std::string name, EntityListBase &list) { b.bind(name, list); }
	*/
	template<type B>
	inline void bind(Binder &b, const std::string name, B &v) { b.bind(name, v); }

	virtual ValueMap validate(ValueMap changes) = 0;

	private:

	EntityDiff diff;

	template<class V>
	V &validate(const std::string &name)
	{
		V *v = new V(*this, name);
		validators.push_back(std::unique_ptr<Validator>(v));
		//return reference for idiom
		return *v;
	}
};

/*
client.createContact()
	.setName("blah")
	.setEmail("blah@bah.com")
	.save();

client.getContact(5)
	.setName("blah")
	.setEmail("blah@bah.com")
	.save();

Contact Client::createContact()
{
	Contact contact = Contact();
	contact.setClientId(getId());
	return contact;
}

bool Contact::validate()
{
	validateEmail("Email");
	validateString("NameFirst")
		.required()
		.matches("^[A-Za-z'\\-]*$")
		.max(50)
		.min(2)
		;
	validatePhoneNumber("Phone");
	validateIntUnsigned("ClientId")
		.required()
		.min(1)
		;
	validateIntUnsigned("Age")
		.min(18)
		.max(150)
		;
	normalize();
}
*/
#endif
