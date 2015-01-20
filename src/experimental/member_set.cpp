#include<type_traits>
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
#define MEMBER(mptr) Member<typename MemberTraits<decltype(mptr)>::OwnerType, typename MemberTraits<decltype(mptr)>::MemberType, mptr>

template<typename... Ms>
struct MemberSet
{
	using Offsets = NumberSet<Ms::offset...>;

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

int main()
{

	using MooMembers = MemberSet<
		MEMBER(&Moo::freq),
		MEMBER(&Moo::duration),
		MEMBER(&Moo::name) >;

	//std::cout << MooMembers::Offsets::template indexOf<Member<Moo, std::string, &Moo::name>::offset>();
	std::cout << MooMembers::template indexOf<MEMBER(&Moo::freq)>();
	std::cout << std::endl;
	std::cout << MooMembers::template indexOf<MEMBER(&Moo::duration)>();
	std::cout << std::endl;
	std::cout << MooMembers::template indexOf<MEMBER(&Moo::name)>();
	std::cout << std::endl;
	using namePtr = MEMBER(&Moo::name);
	Moo moo = Moo();
	moo.name = "fart";	
	std::cout << moo.name;
	std::cout << std::endl;
	std::cout << moo.*namePtr::ptr;
	std::cout << std::endl;
}

