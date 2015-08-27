//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<string>
#include<iostream>
#include<stdexcept>

template<class Owner, typename T> 
struct MemberPtr
{
	using OwnerType = Owner;
	T Owner::* ptr;

	constexpr MemberPtr(T Owner::*p) :
		ptr(p) 
	{}
};
template<class Owner, typename T>
constexpr MemberPtr<Owner, T> memberPtr(T Owner::*ptr)
{
	return MemberPtr<Owner, T>(ptr);
}

template<class Owner, typename... Mptrs>
class MemberMap 
{
	public:
	
	constexpr MemberMap(Mptrs... m) {}

	template<typename T>
	constexpr int map(T Owner::*p, int i = 0)
	{
		return false ? -1 : throw std::out_of_range("Not a mapped member");
	}
};

template<class Owner, typename Mptr, typename... Mptrs>
class MemberMap<Owner, Mptr, Mptrs...> 
	: public MemberMap<Owner, Mptrs...>
{
	const uintptr_t tail;

	public:

	constexpr MemberMap(Mptr mptr1, Mptrs... mptrN) :
		MemberMap<Owner, Mptrs...>(mptrN...),
		tail(offset(mptr1))
	{}

	template<typename T>
	constexpr uintptr_t offset(MemberPtr<Owner, T> p)
	{
		return (const uintptr_t)&((Owner*)nullptr->*p.ptr) - (const uintptr_t)nullptr;
	}

	template<typename T>
	constexpr int map(T Owner::*p, int i = 0)
	{
		return  (offset(memberPtr(p)) == tail) ? i : 
			MemberMap<Owner, Mptrs...>::map(p, i + 1);
	}
};

template<class Mptr, typename... Mptrs>
constexpr MemberMap<typename Mptr::OwnerType, Mptr, Mptrs...>
	memberMap(Mptr p1, Mptrs... pN)
{
	return MemberMap<typename Mptr::OwnerType, Mptr, Mptrs...>(p1, pN...);
}

template<class Impl>
class Base
{
	
};

class Moo : public Base<Moo>
{
	public:
	int frequency;
	int duration;
	std::string cow;
	int outOf;
};

class Fart
{
	public:
	int frequency;
};

template<class Owner, typename T>
constexpr int map(T Owner::*p)
{
	return memberMap(
		memberPtr(&Moo::frequency),
		memberPtr(&Moo::duration),
		memberPtr(&Moo::cow) ).map(p);
}

int main()
{
	/*
	constexpr auto m = memberMap(
		memberPtr(&Moo::frequency),
		memberPtr(&Moo::duration),
		memberPtr(&Moo::cow) );
		*/
	constexpr int x = map(&Moo::outOf) + map(&Moo::duration);
	std::cout << map(&Moo::frequency);
	std::cout << std::endl;
	std::cout << map(&Moo::duration);
	std::cout << std::endl;
	std::cout << map(&Moo::cow);
	std::cout << std::endl;
	std::cout << map(&Moo::outOf);
	std::cout << std::endl;

}
