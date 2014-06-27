inline ValidatorString &
	ValidatorString::minLen(const int l) 
{ 
	if (!chain_broken && value.size() < l) 
		addError("tooShort"); 
	return *this; 
}
inline ValidatorString &
	ValidatorString::maxLen(const int l) 
{ 
	if (!chain_broken && value.size() > l) 
		addError("tooLong"); 
	return *this; 
}
inline ValidatorString &
	ValidatorString::asEmail() 
{ 
	if (!chain_broken)
		matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
	return *this; 
}
inline ValidatorString &
	ValidatorString::allDigits()
{ 
	if (!chain_broken)
		matches("^[0-9]*$"); 
	return *this; 
}
