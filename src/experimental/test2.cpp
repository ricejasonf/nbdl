//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <type_traits>
#include <iostream>
#include <string>

class AnimalBase {};
template<class Derived>
class Animal : public AnimalBase
{
	Derived *derived()
	{
		return static_cast<Derived *>(this);
	}

	public:

	static const bool isAnimal = true;

	void speak()
	{
		std::cout << "The " << derived()->getName() << " says " << derived()->say() << "." << std::endl;
	}
};
class NotAnimal
{
	public:
	void speak()
	{
		std::cout << "This is not an animal.";
	}
};


class Cow : public Animal<Cow>
{
	friend class Animal<Cow>;

	std::string getName()
	{
		return "Cow";
	}

	std::string say()
	{
		return "MOO";
	}
};

template<typename T>
typename std::enable_if<std::is_base_of<AnimalBase, T>::value>::type 
	speak(T t) 
{ 
	t.speak();	
}
template<typename T>
typename std::enable_if<!std::is_base_of<AnimalBase, T>::value>::type 
	speak(T t) 
{ 
	std::cout << "This is not an animal." << std::endl;
}

int main() 
{
	Cow cow;
	speak(cow);
	speak(5);
}
