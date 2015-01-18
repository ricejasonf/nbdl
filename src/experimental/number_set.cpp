#include<iostream>
#include<stdexcept>

template<uintptr_t p1, uintptr_t... pN>
struct NumberSet : NumberSet<pN...>
{
	template<uintptr_t t, int i = 0, bool passed = false> 
	constexpr int indexOf()
	{
		return (t != p1) ? NumberSet<pN...>::template indexOf<t, i + 1, (t == p1 || passed)>() : i;
	}

	constexpr uintptr_t at(int i)
	{
		return (i == 0) ? p1 : NumberSet<pN...>::at(i - 1);
	}

	constexpr int size()
	{
		return sizeof...(pN) + 1;
	}
};

template<uintptr_t p>
struct NumberSet<p>
{
	template<uintptr_t t, int i = 0, bool passed = false> 
	constexpr int indexOf()
	{
		static_assert((t == p || passed), "Not in set");
		return i;
	}

	constexpr uintptr_t at(int i)
	{
		//out of range elements 
		//just assume the last member
		return p;
	}

	constexpr int size()
	{
		return 1;
	}
};

int main()
{
	constexpr NumberSet<0, 4, 8, 12> x;
	std::cout << x.indexOf<0>();
	std::cout << std::endl;
	std::cout << x.indexOf<4>();
	std::cout << std::endl;
	std::cout << x.indexOf<8>();
	std::cout << std::endl;
	std::cout << x.indexOf<12>();
	std::cout << std::endl;
	std::cout << x.at(2);
	std::cout << std::endl;
	std::cout << "size: " << x.size();
	//std::cout << x.indexOf<5>(); //should fail static assert (Not in set.)
}
