#include <string>
#include <boost/regex.hpp>

#include "Validator.h"
#include "Entity.h"

using std::string;
using boost::regex_match;
using boost::regex;

Validator::Validator(Entity &entity, const string &name) :
	entity(entity),
	name(name),
	chain_broken(false)
{
	Entity::ValueMap::iterator i = entity.changedValues.find(name);
	if (i != entity.changedValues.end())
	{
		has_value = true;
		value = (*i).second;
	}
	else
		has_value = false;
}

void Validator::addError(const string &error) 
{ 
	entity.addError(name, error); 
	chain_broken = true; 
}
void Validator::_matches(const string &r, const string &token) 
{ 
	if (!chain_broken && !regex_match(value, regex(r)))
		addError(token); 
}

void Validator::_required()
{
	if (entity.getId())
	{
		if (hasValue() && isBlank())
			addError("required");	
		else if (!hasValue())
			chain_broken = true;
	}
	else if (isBlank())
		addError("required");
}

