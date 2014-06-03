#include <string>

#include "Validator.h"
#include "Entity.h"

using std::string;

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
