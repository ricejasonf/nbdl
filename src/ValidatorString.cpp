#include<string>
#include "ValidatorString.hpp"
//#include "Entity.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
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
ValidatorString & 
ValidatorString::matches(const std::string r, const std::string token) 
{ 
	if (!isChainBroken() 
			&& !boost::regex_match(field, boost::regex(r, boost::regex::ECMAScript)))
		addError(token); 
}
#endif

ValidatorString &
	ValidatorString::minLen(const int l) 
{ 
	if (!isChainBroken() && field.size() < l) 
		addError("tooShort"); 
	return *this; 
}
ValidatorString &
	ValidatorString::maxLen(const int l) 
{ 
	if (!isChainBroken() && field.size() > l) 
		addError("tooLong"); 
	return *this; 
}
ValidatorString &
	ValidatorString::asEmail() 
{ 
	if (!isChainBroken())
		matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
	return *this; 
}
ValidatorString &
	ValidatorString::allDigits()
{ 
	if (!isChainBroken())
		matches("^[0-9]*$"); 
	return *this; 
}
bool 
	ValidatorString::isBlank() 
{ 
	return field.size() > 0;	
}

