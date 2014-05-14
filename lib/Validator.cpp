#include "Validator.h"

Validator::Validator(Entity &entity, const string name) :
	entity(entity),
	name(name),
	chainBroken(false)
{
	Entity::ValueMap::iterator i = entity.changedValues.find(name);
	if (i != entity.changeValues.end())
	{
		_hasValue = true;
		value = *i;
	}
	else
		_hasValue = false;
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

