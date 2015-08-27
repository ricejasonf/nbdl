//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>
#include<string>

template<uintptr_t p1, uintptr_t... pN>
struct NumberSet : NumberSet<pN...>
{
	template<uintptr_t t, int i = 0, bool passed = false> 
	constexpr static int indexOf()
	{
		return (t != p1) ? NumberSet<pN...>::template indexOf<t, i + 1, (t == p1 || passed)>() : i;
	}

	constexpr static uintptr_t at(int i)
	{
		return (i == 0) ? p1 : NumberSet<pN...>::at(i - 1);
	}

	constexpr static int size()
	{
		return sizeof...(pN) + 1;
	}
};

template<uintptr_t p>
struct NumberSet<p>
{
	template<uintptr_t t, int i = 0, bool passed = false> 
	constexpr static int indexOf()
	{
		static_assert((t == p || passed), "Not in set");
		return i;
	}

	constexpr static uintptr_t at(int i)
	{
		//out of range elements 
		//just assume the last member
		return p;
	}

	constexpr static int size()
	{
		return 1;
	}
};

template<typename T>
struct MemberTraits;
template<class Owner, typename T>
struct MemberTraits<T Owner::*>
{
	using OwnerType = Owner;
	using MemberType = T;
};

template<class Owner, typename T, T Owner::*p>
struct Member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
	static constexpr T Owner::*ptr = p;
};
#define NBDL_MEMBER(mptr) Member<typename MemberTraits<decltype(mptr)>::OwnerType, typename MemberTraits<decltype(mptr)>::MemberType, mptr>

template<class Format, class Mptr>
struct MemberName
{
	static constexpr const char *name = "undefined";
};

#define NBDL_MEMBER_NAME(Owner, member_name) \
template<class Format> \
struct MemberName<Format, NBDL_MEMBER(&Owner::member_name)> \
{ static constexpr const char *name = #member_name; };

template<typename M1, typename... Ms>
struct MemberSet
{
	using Offsets = NumberSet<M1::offset, Ms::offset...>;

	template<typename M>
	static constexpr int indexOf()
	{
		return Offsets::template indexOf<M::offset>();
	}
};

struct Moo
{
	int freq;
	int duration;
	std::string name;
};

NBDL_MEMBER_NAME(Moo, freq)
NBDL_MEMBER_NAME(Moo, duration)
NBDL_MEMBER_NAME(Moo, name)

int main()
{

	using MooMembers = MemberSet<
		NBDL_MEMBER(&Moo::freq),
		NBDL_MEMBER(&Moo::duration),
		NBDL_MEMBER(&Moo::name) >;

	//std::cout << MooMembers::Offsets::template indexOf<Member<Moo, std::string, &Moo::name>::offset>();
	std::cout << MooMembers::template indexOf<NBDL_MEMBER(&Moo::freq)>();
	std::cout << std::endl;
	std::cout << MooMembers::template indexOf<NBDL_MEMBER(&Moo::duration)>();
	std::cout << std::endl;
	std::cout << MooMembers::template indexOf<NBDL_MEMBER(&Moo::name)>();
	std::cout << std::endl;
	using namePtr = NBDL_MEMBER(&Moo::name);
	Moo moo = Moo();
	moo.name = "fart";	
	std::cout << moo.name;
	std::cout << std::endl;
	std::cout << moo.*namePtr::ptr;
	std::cout << std::endl;
	std::cout << MemberName<int, NBDL_MEMBER(&Moo::freq)>::name;
	std::cout << std::endl;
	std::cout << MemberName<int, NBDL_MEMBER(&Moo::duration)>::name;
	std::cout << std::endl;
	std::cout << MemberName<int, NBDL_MEMBER(&Moo::name)>::name;
	std::cout << std::endl;
}

