//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef REGEX_HPP
#define REGEX_HPP

#include<string>

namespace nbdl {

struct regex
{
	static bool match(const std::string reg, const std::string field);
};

}//nbdl
#endif
