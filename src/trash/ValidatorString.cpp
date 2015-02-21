#include<string>
#include "ValidatorString.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
template
ValidatorString & 
ValidatorString::matches(const std::string r, const std::string token) 
{ 
	int match = EM_ASM_INT({
		var value = Pointer_stringify($0);
		var regStr = Pointer_stringify($1);
		return value.match(new RegExp(regStr)) !== null;
	}, field.c_str(), r.c_str());
	if (!isChainBroken() && !match)
		addError(token); 
}
#else
#include <boost/regex.hpp>
template
ValidatorString & 
ValidatorString::matches(const std::string r, const std::string token) 
{ 
	if (!isChainBroken() 
			&& !boost::regex_match(field, boost::regex(r, boost::regex::ECMAScript)))
		addError(token); 
}
#endif

template
ValidatorString &
	ValidatorString::minLen(const int l) 
{ 
	if (!isChainBroken() && field.size() < l) 
		addError("tooShort"); 
	return *this; 
}
template
ValidatorString &
	ValidatorString::maxLen(const int l) 
{ 
	if (!isChainBroken() && field.size() > l) 
		addError("tooLong"); 
	return *this; 
}
template
ValidatorString &
	ValidatorString::asEmail() 
{ 
	if (!isChainBroken())
		matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
	return *this; 
}
template
ValidatorString &
	ValidatorString::allDigits()
{ 
	if (!isChainBroken())
		matches("^[0-9]*$"); 
	return *this; 
}
template
bool 
	ValidatorString::isBlank() 
{ 
	return field.size() > 0;	
}

