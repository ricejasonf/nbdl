#ifndef MEMBERSET_HPP
#define MEMBERSET_HPP

#include<iostream>
#include<string>

namespace nbdl {

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

template<class NameFormat, class Mptr>
struct MemberName
{
	static constexpr const char *name = "undefined";
};

#define NBDL_MEMBER_NAME(Owner, member_name) \
template<class Format> \
struct MemberName<Format, NBDL_MEMBER(&Owner::member_name)> \
{ static constexpr const char *name = #member_name; };

template<typename... Mn>
struct MemberSet
{
	template<typename NameFormat, typename Binder, typename OwnerType>
	static void bindMembers(Binder &binder, OwnerType &owner)
	{}
};
template<typename M1, typename... Mn>
struct MemberSet<M1, Mn...>
{
	//todo make M1
	using Offsets = NumberSet<M1::offset, Mn::offset...>;

	template<typename M>
	static constexpr int indexOf()
	{
		return Offsets::template indexOf<M::offset>();
	}

	template<typename NameFormat, typename Binder>
	static void bindMembers(Binder &binder, typename M1::OwnerType &owner)
	{
		binder.bindMember(MemberName<NameFormat, M1>::name, owner.*M1::ptr);
		MemberSet<Mn...>::template bindMembers<NameFormat>(binder, owner);
	}
};

} //nbdl

#endif
