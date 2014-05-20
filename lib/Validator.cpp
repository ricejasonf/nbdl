#include <string>
#include <regex>
#include <iostream>

#include "Validator.h"
#include "Entity.h"

using std::string;
using std::regex_match;

Validator::Validator(Entity &entity, const string &name) :
	entity(entity),
	name(name),
	chainBroken(false)
{
	std::cout << "CONSTRUCTOR FOR Validator CALLED" << std::endl;
	Entity::ValueMap::iterator i = entity.changedValues.find(name);
	if (i != entity.changedValues.end())
	{
		_hasValue = true;
		value = (*i).second;
	}
	else
		_hasValue = false;
}

void Validator::addError(const string &error) 
{ 
	entity.addError(name, error); 
	chainBroken = true; 
}
void Validator::_matches(const string &r, const string &token) 
{ 
	if (!regex_match(value, std::regex(r)))
		addError(token); 
}

void Validator::_required()
{
	if (entity.getId())
	{
		if (hasValue() && isBlank())
			addError("required");	
		else if (!hasValue())
			chainBroken = true;
	}
	else if (isBlank())
		addError("required");
}

