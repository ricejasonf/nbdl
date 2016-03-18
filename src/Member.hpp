//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MEMBER_HPP
#define NBDL_MEMBER_HPP

namespace nbdl {

template<class Owner, typename T, T Owner::*p>
struct member
{
	using OwnerType = Owner;
	using MemberType = T;
	static constexpr T Owner::*ptr = p;
};
template<class M>
struct member_id;

template<typename T>
struct member_traits;
template<class Owner, typename T>
struct member_traits<T Owner::*>
{
	using OwnerType = Owner;
	using MemberType = T;
};

template<class M>
struct member_name;

template<class M>
struct member_default;

template<class M>
struct member_has_default
{
	static const bool value = false;
};

template<class M>
struct member_string_max_length
{
	//default max length of 50 for all strings
	static const int value = 50;
};

template<class M>
struct member_string_min_length
{
	static const int value = 0;
};

template<class M>
struct member_match
{
	static constexpr const char *value = nullptr;
};

//allow a string to have zero length
template<class M>
struct member_allow_blank
{
	static const bool value = false;
};

//string treated as buffer and does no trim filtering
template<class M>
struct member_raw_buffer
{
	static const bool value = false;
};

template<class M>
struct member_custom_validator
{
	template<typename T, typename AddError>
	static void validate(T&, AddError) {}
};

}//nbdl

/*
 * MACROS
 */
#define NBDL_MEMBER(mptr) ::nbdl::member<typename ::nbdl::member_traits<decltype(mptr)>::OwnerType, typename ::nbdl::member_traits<decltype(mptr)>::MemberType, mptr>

#define NBDL_MEMBER_ID(mptr, v) template<> struct member_id<NBDL_MEMBER(mptr)> \
{ static const uint8_t value = v; };

#define NBDL_MEMBER_NAME(Owner, MemberName) \
template<> \
struct member_name<NBDL_MEMBER(&Owner::MemberName)> \
{ static constexpr const char *value = #MemberName; };

#define NBDL_MEMBER_DEFAULT(mptr, val) template<> struct member_default<NBDL_MEMBER(mptr)> \
{ static constexpr decltype(val) value = val; }; \
template<> struct member_has_default<NBDL_MEMBER(mptr)> \
{ static const bool value = true; }; 

#define NBDL_MEMBER_MAXLENGTH(mptr, v) template<> struct member_string_max_length<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MINLENGTH(mptr, v) template<> struct member_string_min_length<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MATCH(mptr, reg) template<> struct member_match<NBDL_MEMBER(mptr)> \
{ static constexpr const char *value = reg; };

#define NBDL_MEMBER_ALLOWBLANK(mptr) template<> struct member_allow_blank<NBDL_MEMBER(mptr)> \
{ static const bool value = true; };


#endif
