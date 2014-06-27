inline ValidatorString &
	ValidatorString::minLen(const int m) 
{ 
	if (!chain_broken && value.size() < m) 
		addError("tooShort"); 
	return *this; 
}
inline ValidatorString &
	ValidatorString::maxLen(const int m) 
{ 
	if (!chain_broken && value.size() > m) 
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
