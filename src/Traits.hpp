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
struct entity_traits;

template<class T, class U = void>
struct is_entity
{
	enum { value = 0 };
};
template<class T>
struct is_entity<T, typename Void<typename entity_traits<typename std::decay<T>::type>::Members>::type>
{
	enum { value = 1 };
};

template<typename T, typename U = void>
using enable_if_entity = std::enable_if_t<is_entity<T>::value, U>;

template<typename T, typename U = void>
using enable_if_empty = std::enable_if_t<std::is_empty<T>::value, U>;

template<typename E>
static constexpr auto entity_members()
{
  return typename entity_traits<typename std::decay<E>::type>::Members{};
}

//returns reference to member
template<typename E, typename T>
static auto& entity_member(E&& entity, T member_type)
{
  using Member_ = typename decltype(member_type)::type;
  return std::forward<E>(entity).*Member_::ptr;
}

//Traits for Member

template<typename T>
static constexpr auto member_name_(T)
{
  return member_name<typename T::type>::value;
}

//Traits for Variant

template<class T, class U = void>
struct is_variant
{
	enum { value = 0 };
};
template<class T>
struct is_variant<T, typename Void<typename std::decay<T>::type::VariantTag>::type>
{
	enum { value = 1 };
};

template<typename T, typename U = void>
using enable_if_variant = std::enable_if_t<is_variant<T>::value, U>;

}//nbdl

#endif
