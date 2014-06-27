#include "ValidatorString.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
void Validator::_matches(const std::string &r, const std::string &token) 
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
void Validator::_matches(const std::string &r, const std::string &token) 
{ 
	if (!chain_broken 
			&& !boost::regex_match(value, boost::regex(r, boost::regex::ECMAScript)))
		addError(token); 
}
