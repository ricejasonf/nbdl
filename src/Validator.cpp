#include <string>

#include "Validator.h"
#include "Entity.h"

using std::string;

template<typename T>
Validator<T>Validator(Entity &entity, T &field) :
	entity(entity),
	field(field),
	chain_broken(false)
{
	if (entity.isDirty(field))
	Entity::ValueMap::iterator i = entity.changedValues.find(name);
	if (i != entity.changedValues.end())
	{
		has_value = true;
		value = (*i).second;
	}
	else
		has_value = false;
}

template<typename T>
void Validator<T>addError(const string &error) 
{ 
	entity.addError(name, error); 
	chain_broken = true; 
}

template<typename T>
void Validator<T>_required()
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

template<typename T>
bool Validator<T>isBlank() { return false; }
template<>
bool Validator<std::string>::isBlank() 
{ 
	return field.size() > 0;	
}

//todo move to ValidatorString.cpp
#ifdef EMSCRIPTEN
#include <emscripten.h>
void Validator::_matches(const string &r, const string &token) 
{ 
	int matches = EM_ASM_INT({
		var value = Pointer_stringify($0);
		var regStr = Pointer_stringify($1);
		return value.match(new RegExp(regStr)) !== null;
	}, value.c_str(), r.c_str());
	if (!chain_broken && !matches)
		addError(token); 
}
#else
#include <boost/regex.hpp>
void Validator::_matches(const string &r, const string &token) 
{ 
	if (!chain_broken 
			&& !boost::regex_match(value, boost::regex(r, boost::regex::ECMAScript)))
		addError(token); 
}
#endif
