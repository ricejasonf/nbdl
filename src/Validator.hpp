#include "Entity.h"

tempate<class C, typename T>
inline void 
	Validator<C, T>::addError(const std::string &error) 
{ 
	errors.addError(field, error);
	chain_broken = true; 
}

tempate<class C, typename T>
inline C& 
	Validator<C, T>::required()
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

tempate<class C, typename T>
inline C& 
	Validator<C, T>::_optional() 
{ 
	if (entity.isDirty(field)) 
		chain_broken = true; 
}

tempate<class C, typename T>
inline C& 
	Validator<C, T>::inSet(const std::vector<T> set) 
{
	if (!chain_broken && std::find(set.begin(), set.end(), value) == set.end())
		addError("notInSet");
}

tempate<class C, typename T>
inline C& 
	Validator<C, T>::inSet(const std::unordered_set<T> set) 
{
	if (!chain_broken && set.find(field) != set.end())
		addError("notInSet");
}

tempate<class C, typename T>
inline bool 
	Validator<C, T>::isBlank() 
{ 
	return false; 
}

template<class C>
inline bool 
	Validator<C, std::string>::isBlank() 
{ 
	return field.size() > 0;	
}

#endif
