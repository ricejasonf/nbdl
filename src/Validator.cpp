#include <string>

#include "Validator.h"
#include "Entity.h"

template<typename T>
Validator<T>Validator(Entity &entity, T &field) :
	entity(entity),
	field(field),
	chain_broken(false) {}

template<typename T>
void Validator<T>addError(const std::string &error) 
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
#endif
