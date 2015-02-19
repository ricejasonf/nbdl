#ifndef REGEX_HPP
#define REGEX_HPP

#include<string>

namespace nbdl {

struct Regex
{
	static bool match(const std::string reg, const std::string field);
};

}//nbdl
#endif
