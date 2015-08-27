//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "Regex.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
bool nbdl::Regex::match(const std::string reg, const std::string field)
{ 
	int match = EM_ASM_INT({
		var value = Pointer_stringify($0);
		var regStr = Pointer_stringify($1);
		return value.match(new RegExp(regStr)) !== null;
	}, field.c_str(), reg.c_str());
	return (match != 0);
}
#else
#include <boost/regex.hpp>
bool nbdl::Regex::match(const std::string reg, const std::string field)
{ 
	 return boost::regex_match(field, boost::regex(reg, boost::regex::ECMAScript));
}
#endif //ifdef EMSCRIPTEN
