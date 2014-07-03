#include<algorithm>

template<typename T>
inline void 
Entity::set(T value, T &field)
{
	diff.set(value, field, this);
}

inline void 
Entity::applyDiff(Binder &binder)
{
	binder.setDiffMode(true);	
	bindMembers(binder);
}

template<typename T>
inline ValidatorNumber<T>
Entity::validateNumber(ErrorBinder &e, T &field)
{ 
	return ValidatorNumber<T>(*this, field, e);
}

inline ValidatorString
Entity::validateString(ErrorBinder &e, std::string &field)
{ 
	return ValidatorString(*this, field, e);
}

template<typename T>
inline void 
Entity::bind(Binder &b, const std::string name, T &field) 
{ 
	b.bind(*this, name, field); 
}
template<typename T>
inline void 
Entity::bindReadOnly(Binder &b, const std::string name, T &field) 
{
	if (b.diffMode()) 
		return;
	bind(b, name, field);
}
inline void 
Entity::bindPathKey(Binder &b, const std::string name, unsigned int &field) 
{
	//prevents modifying a key that is already established
	//this should be used for every key in the path
	if (b.diffMode() && field > 0) 
		return;
	bind(b, name, field);
}

template<typename T>
inline void 
Entity::Diff::set(T value, T& field, Entity *container)
{
	field = value;
	dirtyFields.push_back((uintptr_t)&field - (uintptr_t)container);
}

inline bool
Entity::isDirty()
{
	return diff.isDirty();
}

inline bool
Entity::Diff::isDirty()
{
	return dirtyFields.size() > 0;
}
template<typename T>
inline bool 
Entity::isDirty(T &field)
{
	return diff.isDirty(field, this);
}

template<typename T>
inline bool
Entity::Diff::isDirty(T &field, Entity *container)
{
	return std::find(
		dirtyFields.begin(),
		dirtyFields.end(),
		(uintptr_t)&field - (uintptr_t)container) != dirtyFields.end();
}

template<typename T>
inline void 
Binder::set(Entity &e, T value, T &field) { e.set(value, field); }

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
