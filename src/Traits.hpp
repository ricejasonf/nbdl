//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TRAITS_HPP
#define NBDL_TRAITS_HPP

#include "Member.hpp"

#include<utility>
#include<type_traits>

namespace nbdl {

template<typename T>
struct Void
{
	using type = void;
};

//Traits for Entity

template<typename Impl>
struct EntityTraits;

template<class T, class U = void>
struct IsEntity
{
	enum { value = 0 };
};
template<class T>
struct IsEntity<T, typename Void<typename EntityTraits<typename std::decay<T>::type>::Members>::type>
{
	enum { value = 1 };
};

template<typename T, typename U = void>
using EnableIfEntity = std::enable_if_t<IsEntity<T>::value, U>;

template<typename T, typename U = void>
using EnableIfEmpty = std::enable_if_t<std::is_empty<T>::value, U>;

template<typename E>
static constexpr auto entityMembers()
{
  return typename EntityTraits<typename std::decay<E>::type>::Members{};
}

//returns reference to member
template<typename E, typename T>
static auto& entityMember(E&& entity, T member_type)
{
  using Member_ = typename decltype(member_type)::type;
  return std::forward<E>(entity).*Member_::ptr;
}

//Traits for Member

template<typename T>
static constexpr auto memberName(T)
{
  return MemberName<typename T::type>::value;
}

//Traits for Variant

template<class T, class U = void>
struct IsVariant
{
	enum { value = 0 };
};
template<class T>
struct IsVariant<T, typename Void<typename std::decay<T>::type::VariantTag>::type>
{
	enum { value = 1 };
};

template<typename T, typename U = void>
using EnableIfVariant = std::enable_if_t<IsVariant<T>::value, U>;

}//nbdl

#endif
