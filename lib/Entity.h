#ifndef ENTITY_H
#define ENTITY_H

#include "ValidatorInt.h"
#include "ValidatorIntUnsigned.h"
#include "ValidatorString.h"

use std::string;
use std::vector;
use std::map;

class Entity
{
	public:

	typedef unordered_map<string, string> ValueMap;
	typedef Validator<string, vector, ValueMap> Validator;

	Entity(ValueMaP &values) :
		values(values) {}
	virtual ~Entity() { clearValidators(); }

	int getId();

	virtual void create(); 
	virtual void read();
	virtual void update();
	virtual void remove();

	void save();

	inline void set(const string name, const string value) { changedValues[name] = value; }
	const string get(const string name);
	
	//add accessors in child?

	protected:

	inline ValidatorIntUnsigned &validateIntUnsigned(const string &name) { return validate<ValidatorIntUnsigned>(name); }
	inline ValidatorInt &validateInt(const string &name) { return validate<ValidatorInt>(name); }
	inline ValidatorString &validateString(const string &name) { return validate<ValidatorString>(name); }

	virtual void validate() {}

	virtual const string getRevisionResourceKey() = 0;
	virtual const string getRevisionObjectType() = 0;

	void notifyChange(const ValueMap diff);
	void notifyChange(const ValueMap diff, int id);

	inline void addValidator(Validator *v) { validators.push_back(v); }
	void addError(const string name, const string *e);

	private:

	friend class Validator;

	void clearValidators();

	ValueMap &values;
	ValueMap changedValues;
	vector<Validator *> validators;
	map<string, vector<string>> errors;
	<template class V>
	V &validate(const string &name);

}

#endif

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
