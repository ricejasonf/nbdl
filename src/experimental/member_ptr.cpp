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

int main()
{
	A a;
	printMember(a, &A::m);
	std::cout << offset(getMember1(), getMember2());
	std::cout << std::endl;
}
