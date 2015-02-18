#include "Regex.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
bool nbdl::Regex::match(const std::string reg, const std::string field);
{ 
	int match = EM_ASM_INT({
		var value = Pointer_stringify($0);
		var regStr = Pointer_stringify($1);
		return value.match(new RegExp(regStr)) !== null;
	}, field.c_str(), r.c_str());
	return (match != 0);
}
#else
#include <boost/regex.hpp>
bool nbdl::Regex::match(const std::string reg, const std::string field)
{ 
	 return boost::regex_match(field, boost::regex(reg, boost::regex::ECMAScript));
}
#endif //ifdef EMSCRIPTEN
