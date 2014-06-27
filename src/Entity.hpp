#include <string>

#include "ValidatorNumber.h"
#include "ValidatorString.h"

inline void 
	Entity::Binder::bindMembers(Entity &e) 
{ 
	e.bindMembers(*this); 
}

template<typename T>
void 
	Entity::set(T value, T &field)
{
	diff.set(value, field, this);
}

template<typename T>
inline ValidatorNumber &
	validateNumber(T &field, Errors &e) 
{ 
	return validate<T, ValidatorNumber>(field, e); 
}
inline ValidatorString &
	validateString(T &field, Errors &e)
{ 
	return validate<std::string, ValidatorString>(field, e); 
}

template<typename T, class V>
V
	Entity::validate(T &field, Errors &e)
{
	return V(*this, field, e);
}

template<type T>
inline void 
	Entity::bind(Binder &b, const std::string name, T &field) 
{ 
	b.bind(name, field); 
}


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
