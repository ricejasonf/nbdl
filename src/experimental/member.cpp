//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>

struct Moo
{
    int freq;
    int duration;
};

template<class Owner, typename T, T Owner::*p>
struct Member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
};

int main()
{
	std::cout << Member<Moo, int, &Moo::duration>::offset;
	std::cout << std::endl;	
}
