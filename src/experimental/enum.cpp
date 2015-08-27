//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<string>
#include<iostream>

//what the heck was i trying to accomplish here?

namespace { 
extern const char * const statuses[] = {
	"pending",
	"still_pending",
	"just_paid",
	"archived",
	"just_stale_and_old"
};
}


template<int N>
constexpr int enum_type_size(const char * const (&v)[N])
{
	return N;
}

template<int N, const char * const (&v)[N]>
struct enum_type
{
	static constexpr const char * const *values = v;

	static constexpr int size = enum_type_size(v);
};

int main()
{
	std::cout << enum_type<enum_type_size(statuses), statuses>::values[2];
	std::cout << std::endl;
	std::cout << enum_type<enum_type_size(statuses), statuses>::size;
	std::cout << std::endl;
}
