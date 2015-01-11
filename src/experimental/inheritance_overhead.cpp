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
