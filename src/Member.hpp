#ifndef NBDL_MEMBER_HPP
#define NBDL_MEMBER_HPP

#include<string>

namespace nbdl {

template<class Owner, typename T, T Owner::*p>
struct Member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr uintptr_t offset = (uintptr_t)&((Owner*)nullptr->*p);
	static constexpr T Owner::*ptr = p;
};

template<typename T>
struct MemberTraits;
template<class Owner, typename T>
struct MemberTraits<T Owner::*>
{
	using OwnerType = Owner;
	using MemberType = T;
};

template<class NameFormat, class Mptr>
struct MemberName;

template<class M>
struct MemberDefault;

template<class M>
struct MemberStringMaxLength
{
	//default max length of 50 for all strings
	static const int value = 50;
};

}//nbdl

/*
 * MACROS
 */
#define NBDL_MEMBER(mptr) Member<typename MemberTraits<decltype(mptr)>::OwnerType, typename MemberTraits<decltype(mptr)>::MemberType, mptr>

#define NBDL_MEMBER_NAME(Owner, member_name) \
template<class Format> \
struct MemberName<Format, NBDL_MEMBER(&Owner::member_name)> \
{ static constexpr const char *name = #member_name; };

#define NBDL_MEMBER_DEFAULT(mptr, val) template<> struct MemberDefault<NBDL_MEMBER(mptr)> \
{ static const decltype(NBDL_MEMBER(mptr)::MemberType) value = val; };

#define NBDL_MEMBER_MAXLENGTH(mptr) template<> struct MemberStringMaxLength<NBDL_MEMBER(mptr, v)> \
{ static const int value = v; };


#endif
