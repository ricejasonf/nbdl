#include<type_traits>
#include<iostream>
#include<string>
#include<cstddef>

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

template<class Owner, typename T, T Owner::*p>
struct Member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
};

template<typename... Ms>
struct MemberSet
{
	using Offsets = NumberSet<Ms::offset...>;

};

struct Moo
{
	int freq;
	int duration;
	std::string name;
};

int main()
{
	typedef NumberSet<0, 4, 8, 12> X;
	std::cout << X::indexOf<0>();
	std::cout << std::endl;
	std::cout << X::indexOf<4>();
	std::cout << std::endl;
	std::cout << X::indexOf<8>();
	std::cout << std::endl;
	std::cout << X::indexOf<12>();
	std::cout << std::endl;
	std::cout << X::at(2);
	std::cout << std::endl;
	std::cout << "size: " << X::size();
	std::cout << std::endl;
	//std::cout << X::indexOf<5>(); //should fail static assert (Not in set.)

	using MooMembers = MemberSet<
		Member<Moo, int, &Moo::freq>,
		Member<Moo, int, &Moo::duration>,
		Member<Moo, std::string, &Moo::name>>;

	std::cout << MooMembers::Offsets::template indexOf<Member<Moo, std::string, &Moo::name>::offset>();
	std::cout << std::endl;
}
