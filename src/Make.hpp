//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_HPP
#define NBDL_MAKE_HPP

#include<type_traits>
#include<utility>

namespace nbdl {

//general high order make function
template<template<class...> class T>
struct Make
{
  template<typename... X>
  constexpr auto operator()(X... x) const
  {
    return T<std::decay_t<decltype(x)>...>
      (std::forward<decltype(x)>(x)...);
  }
};
template<template<class...> class T>
constexpr Make<T> make{};

}//nbdl

#endif
