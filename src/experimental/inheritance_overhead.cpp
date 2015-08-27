//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>

class Base 
{
	int moo;
	char f;
};

class Impl : public Base
{
};


int main()
{
	Base base;
	Impl impl;
	std::cout << sizeof(Base);
	std::cout << std::endl;
	std::cout << sizeof(Impl);
	std::cout << std::endl;
}
