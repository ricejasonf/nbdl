//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>

template<typename T, Enable = void>
struct HasType;
template<typename T, typename T::>
struct HasType;


struct Moo 
{
	using MyType = int;
};
struct NotMoo;

template<typename T, typename Enable = void>
struct Animal
{
	static void speak()
	{
		std::cout << "Not a cow.";
		std::cout << std::endl;
	}
};

template<typename T>
struct Animal<T, typename HasType<T::MyType>>
{
	static void speak()
	{
		std::cout << "moooooOOOOOO!";
		std::cout << std::endl;
	}
};

int main()
{
	Animal<NotMoo>::speak();
	Animal<Moo>::speak();
}
