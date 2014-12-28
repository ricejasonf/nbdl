#ifdef EMSCRIPTEN
#include <emscripten.h>
bool match(const std::string reg, const std::string field);
{ 
	int match = EM_ASM_INT({
		var value = Pointer_stringify($0);
		var regStr = Pointer_stringify($1);
		return value.match(new RegExp(regStr)) !== null;
	}, field.c_str(), r.c_str());
	if (!this->isChainBroken() && !match)
		this->addError(token); 
}
#else
#include <boost/regex.hpp>
bool match(const std::string reg, const std::string field);
{ 
	if (!this->isChainBroken() 
			&& !boost::regex_match(field, boost::regex(r, boost::regex::ECMAScript)))
		this->addError(token); 
}
#endif //ifdef EMSCRIPTEN
