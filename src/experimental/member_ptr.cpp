//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>
#include<string>

struct A
{
	A() : m("moo") {}
	int i;
	std::string m;	
};

template<class T, typename MemberPtr>
void printMember(T &t, MemberPtr p)
{
	std::cout << t.*p << std::endl;
}

template<class Owner, class T1, class T2>
constexpr int offset(T1 Owner::*p1, T2 Owner::*p2)
{
	return (uintptr_t)&((Owner*)nullptr->*p1) - (uintptr_t)&((Owner*)nullptr->*p2);
}

constexpr std::string A::* getMember1()
{
	return &A::m;
}
constexpr int A::* getMember2()
{
	return &A::i;
}

void changeToFoo(A &owner, std::string A::*mptr)
{
	owner.*mptr = "foo";
}

int main()
{
	A a;
	std::string A::*mptr = &A::m;
	changeToFoo(a, mptr);
	printMember(a, &A::m);
	std::cout << offset(getMember1(), getMember2());
	std::cout << std::endl;
}
