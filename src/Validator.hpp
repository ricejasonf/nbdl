template<class C, typename T>
inline void 
	Validator<C, T>::addError(const std::string error) 
{ 
	errorBinder.addError(entity, field, error);
	chain_broken = true; 
}

template<class C, typename T>
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
	return static_cast<C&>(*this);
}

template<class C, typename T>
inline C& 
	Validator<C, T>::optional() 
{ 
	if (entity.isDirty(field)) 
		chain_broken = true; 
	return static_cast<C&>(*this);
}

template<class C, typename T>
inline C& 
	Validator<C, T>::inSet(const std::vector<T> set) 
{
	if (!chain_broken && std::find(set.begin(), set.end(), field) == set.end())
		addError("notInSet");
	return static_cast<C&>(*this);
}

template<class C, typename T>
inline C& 
	Validator<C, T>::inSet(const std::unordered_set<T> set) 
{
	if (!chain_broken && set.find(field) != set.end())
		addError("notInSet");
	return static_cast<C&>(*this);
}

template<class C, typename T>
inline bool 
	Validator<C, T>::isBlank() 
{ 
	return false; 
}
