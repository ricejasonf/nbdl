//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENTITY_MEMBERS_HPP
#define NBDL_FWD_ENTITY_MEMBERS_HPP

#include <nbdl/macros/NBDL_ENTITY.hpp>

#include <type_traits>

namespace nbdl
{
  template <typename E>
  struct entity_members_impl;

  template <typename E>
  constexpr typename entity_members_impl<std::decay_t<E>>::type entity_members{};

  template <typename Member>
  struct get_member_fn
  {
    template <typename Entity>
    decltype(auto) operator()(Entity&& e) const;
  };

  template <typename Member>
  constexpr get_member_fn<Member> get_member{};

  template <class Owner, typename T, T Owner::*p>
  struct member
  {
    using owner_type = Owner;
    using member_type = T;
    static constexpr T Owner::*ptr = p;
  };

  template <typename T>
  struct member_traits;
  template <class Owner, typename T>
  struct member_traits<T Owner::*>
  {
    using owner_type = Owner;
    using member_type = T;
  };

  template <class M>
  struct member_name_impl;

  template <class M>
  constexpr char const* member_name = member_name_impl<M>::value;

  template <class M>
  struct member_default;

  template <class M>
  struct member_string_max_length
  {
    //default max length of 50 for all strings
    static const int value = 50;
  };

  template <class M>
  struct member_string_min_length
  {
    static const int value = 0;
  };

  template <class M>
  struct member_match
  {
    // TODO figure out if this really needs to be a nullptr
    static constexpr const char *value = nullptr;
  };

  //allow a string to have zero length
  template <class M>
  struct member_allow_blank
  {
    static const bool value = false;
  };

  //string treated as buffer and does no trim filtering
  template <class M>
  struct member_raw_buffer
  {
    static const bool value = false;
  };

  template <class M>
  struct member_custom_validator
  {
    template <typename T, typename AddError>
    static void validate(T&, AddError) {}
  };

}//nbdl

/*
 * MEMBER MACROS
 */
#define NBDL_MEMBER(mptr) ::nbdl::member<typename ::nbdl::member_traits<decltype(mptr)>::owner_type, typename ::nbdl::member_traits<decltype(mptr)>::member_type, mptr>

#define NBDL_MEMBER_NAME(Owner, MemberName) \
template <> \
struct member_name_impl<NBDL_MEMBER(&Owner::MemberName)> \
{ \
  static constexpr char const* const value = #MemberName; \
  static constexpr std::size_t length = sizeof(#MemberName) - 1; \
};

#define NBDL_MEMBER_DEFAULT(mptr, val) template <> struct member_default<NBDL_MEMBER(mptr)> \
{ static constexpr decltype(val) value = val; }; \

#define NBDL_MEMBER_MAXLENGTH(mptr, v) template <> struct member_string_max_length<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MINLENGTH(mptr, v) template <> struct member_string_min_length<NBDL_MEMBER(mptr)> \
{ static const unsigned value = v; };

#define NBDL_MEMBER_MATCH(mptr, reg) template <> struct member_match<NBDL_MEMBER(mptr)> \
{ static constexpr const char *value = reg; };

#define NBDL_MEMBER_ALLOWBLANK(mptr) template <> struct member_allow_blank<NBDL_MEMBER(mptr)> \
{ static const bool value = true; };

#endif
