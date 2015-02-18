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

struct DefaultNameFormat {};

template<class NameFormat, class M>
struct MemberName;

//overlapping specialization problem here
template<class M>
struct MemberName<int, M>
{
	static constexpr int value = EntityTraits<typename M::OwnerType>::Members::template indexOf<M>();
};

template<class M>
struct MemberDefault;

template<class M>
struct MemberStringMaxLength
{
	//default max length of 50 for all strings
	static const int value = 50;
};

template<class M>
struct MemberStringMinLength;

template<class M>
struct MemberMatch;

//allow a string to have zero length
template<class M>
struct MemberAllowBlank;

//string treated as buffer and does no trim filtering
template<class M>
struct MemberRawBuffer;

}//nbdl

/*
 * MACROS
 */
#define NBDL_MEMBER(mptr) Member<typename MemberTraits<decltype(mptr)>::OwnerType, typename MemberTraits<decltype(mptr)>::MemberType, mptr>

#define NBDL_MEMBER_NAME(Owner, member_name) \
template<> \
struct MemberName<DefaultNameFormat, NBDL_MEMBER(&Owner::member_name)> \
{ static constexpr const char *value = #member_name; };

#define NBDL_MEMBER_DEFAULT(mptr, val) template<> struct MemberDefault<NBDL_MEMBER(mptr)> \
{ static constexpr decltype(val) value = val; };

#define NBDL_MEMBER_MAXLENGTH(mptr, v) template<> struct MemberStringMaxLength<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MINLENGTH(mptr, v) template<> struct MemberStringMinLength<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MATCH(mptr, reg) template<> struct MemberMatch<NBDL_MEMBER(mptr)> \
{ static constexpr const char *value = reg; };


#endif
