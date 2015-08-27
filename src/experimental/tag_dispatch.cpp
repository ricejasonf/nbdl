//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>
#include<string>

struct Cow {};
struct Dog {};

void speak(const int x, Cow)
{
	std::cout << "moooOOOO" << std::endl;
}
void speak(const int x, Dog)
{
	std::cout << "woof" << std::endl;
}

int main()
{
	speak(5, Cow{});
	speak(5, Dog{});
}
