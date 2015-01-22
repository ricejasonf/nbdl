#include<typeinfo>
#include<iostream>
#include<string>

template<typename T>
struct MemberTraits;
template<class Owner, typename T>
struct MemberTraits<T Owner::*>
{
	using OwnerType = Owner;
	using MemberType = T;
	//static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
};

template<typename Traits>
struct Member<T Owner::*p>
{
	static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
}

template<typename Owner>
struct MemberMap
{
	template<typename T>
	constexpr Member<Owner, p> member(T Owner::*p)
	{
		return Member<Owner, T, (uintptr_t)&((Owner*)nullptr->*p)>();
	}
}

/*
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
*/

struct Moo : MemberMap<Moo>
{
	int freq;
	int duration;
	std::string name;
};

int main()
{
	//auto x = member(&Moo::duration);
	//std::cout << typeid(x).name();
	//std::cout << std::endl;
	//std::cout << MemberTraits<decltype(&Moo::duration)>::offset;
	//std::cout << std::endl;

	Moo moo = Moo();
	using X = decltype(moo);
	std::cout << X::offset;
	std::cout << std::endl;
}
