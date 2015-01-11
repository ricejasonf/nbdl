#include<string>
#include<iostream>

template<class Owner, typename... MemberTypes>
class MemberMap {};

template<class Owner, typename MemberType, typename... MemberTypes>
class MemberMap<Owner, MemberType, MemberTypes...> : public MemberMap<Owner, MemberTypes...>
{
	MemberType Owner::*tail;

	public:

	template<typename... MemberPtrs>
	constexpr MemberMap(MemberType Owner::*m, MemberPtrs... ptrs) :
		MemberMap<Owner, MemberTypes...>(ptrs...)
		tail(m)
	{}

	/*
	template<typename MemberPtr>
	constexpr int map(Owner Mtype::*p, int i = 0)
	{
		return ((uintptr_t)&((Owner*)nullptr->*p) - (uintptr_t)nullptr) 
			== thisOffset() ? i : 
			
	}
	constexpr int thisOffset()
	{
		return (uintptr_t)&((Owner*)nullptr->*tail) - (uintptr_t)nullptr;
	}
	*/
};

template<class Owner, typename MemberType, typename... Args>
MemberMap<Owner, MemberType, Args...>
	memberMap(MemberType Owner::*p1, Args... args)
{
	MemberMap<Owner, MemberType, Args...> moo(p1, args...);
	return moo;
	//return MemberMap<Owner, MemberType, Args...>moo(&Owner::p1, Args... args);
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
};

int main()
{
	auto m = memberMap(
		&Moo::frequency,
		&Moo::duration,
		&Moo::cow);
}
