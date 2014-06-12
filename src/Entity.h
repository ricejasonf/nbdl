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

class Entity
{
	public:

	typedef std::shared_ptr<Entity> Ptr;
	typedef std::unordered_map<std::string, std::string> ValueMap;
	typedef std::unordered_map<std::string, std::vector<std::string>> ErrorList;

	Entity(std::unique_ptr<BackEnd> backEnd) :
		backEnd(std::move(backEnd)) {}
	Entity(Entity&&); //move constructor
	virtual ~Entity() { }

	int getId() { return id; }
	bool save();

	const ErrorList getErrors() { return errors; }

	friend class Validator;
	friend class EntityBuilder;
	friend class Binder;
	class Binder
	{
		protected:

		friend class Entity;
		virtual void bind(const std::string, Entity &) = 0;
		virtual void bind(const std::string, std::vector<Entity> &) = 0;
		//todo bind members of primitive type if the ol' string vector thing doesn't cut it

		inline void bindMembers(Entity &e) { e.bindMembers(*this); };
		inline ValueMap getValues(Entity &e) { return e.values; };
		inline void initValue(Entity &entity, const std::string name, const std::string value) { entity.initValue(name, value); }
	};

	protected:

	virtual void bindMembers(Binder &) {}

	inline void set(const std::string name, const std::string value) { changedValues[name] = value; }
	const std::string get(const std::string &name);
	inline void initValue(const std::string name, const std::string value) { values[name] = value; }
	void addError(const std::string &name, const std::string &error);

	inline ValidatorIntUnsigned &validateIntUnsigned(const std::string name) { return validate<ValidatorIntUnsigned>(name); }
	inline ValidatorInt &validateInt(const std::string name) { return validate<ValidatorInt>(name); }
	inline ValidatorString &validateString(const std::string name) { return validate<ValidatorString>(name); }

	inline void bind(Binder &b, const std::string name, Entity &entity) { b.bind(name, entity); }
	inline void bind(Binder &b, const std::string name, std::vector<Entity> &list) { b.bind(name, list); }

	virtual void validate() = 0;

	//Builder &builder;
	std::unique_ptr<BackEnd> backEnd;

	private:

	void flushValidatedValues();

	ValueMap values;
	ValueMap changedValues;
	ValueMap validatedValues;
	Validator *currentValidator;
	std::vector<std::unique_ptr<Validator> > validators;
	ErrorList errors;
	bool has_errors;
	int id;

	template<class V>
	V &validate(const std::string &name)
	{
		V *v = new V(*this, name);
		validators.push_back(std::unique_ptr<Validator>(v));
		//return reference for idiom
		return *v;
	}

};

#endif

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
