#ifndef NBDL_MEMBER_HPP
#define NBDL_MEMBER_HPP

namespace nbdl {

template<class Owner, typename T, T Owner::*p>
struct Member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr T Owner::*ptr = p;
};
template<class M>
struct MemberId;

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
struct MemberHasDefault
{
	static const bool value = false;
};

template<class M>
struct MemberStringMaxLength
{
	//default max length of 50 for all strings
	static const int value = 50;
};

template<class M>
struct MemberStringMinLength
{
	static const int value = 0;
};

template<class M>
struct MemberMatch
{
	static constexpr const char *value = nullptr;
};

//allow a string to have zero length
template<class M>
struct MemberAllowBlank
{
	static const bool value = false;
};

//string treated as buffer and does no trim filtering
template<class M>
struct MemberRawBuffer
{
	static const bool value = false;
};

template<class M>
struct MemberCustomValidator
{
	template<typename T, typename AddError>
	static void validate(T&, AddError) {}
};

}//nbdl

/*
 * MACROS
 */
#define NBDL_MEMBER(mptr) ::nbdl::Member<typename ::nbdl::MemberTraits<decltype(mptr)>::OwnerType, typename ::nbdl::MemberTraits<decltype(mptr)>::MemberType, mptr>

#define NBDL_MEMBER_ID(mptr, v) template<> struct MemberId<NBDL_MEMBER(mptr)> \
{ static const uint8_t value = v; };

#define NBDL_MEMBER_NAME(Owner, member_name) \
template<> \
struct MemberName<DefaultNameFormat, NBDL_MEMBER(&Owner::member_name)> \
{ static constexpr const char *value = #member_name; };

#define NBDL_MEMBER_DEFAULT(mptr, val) template<> struct MemberDefault<NBDL_MEMBER(mptr)> \
{ static constexpr decltype(val) value = val; }; \
template<> struct MemberHasDefault<NBDL_MEMBER(mptr)> \
{ static const bool value = true; }; 

#define NBDL_MEMBER_MAXLENGTH(mptr, v) template<> struct MemberStringMaxLength<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MINLENGTH(mptr, v) template<> struct MemberStringMinLength<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MATCH(mptr, reg) template<> struct MemberMatch<NBDL_MEMBER(mptr)> \
{ static constexpr const char *value = reg; };

#define NBDL_MEMBER_ALLOWBLANK(mptr) template<> struct MemberAllowBlank<NBDL_MEMBER(mptr)> \
{ static const bool value = true; };


#endif
