//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MPDEF_MAKE_METASTRUCT_HPP
#define MPDEF_MAKE_METASTRUCT_HPP

namespace mpdef {

// A Metastruct is just a struct with methods that return 
// default-constructible empty objects like hana::type
template<template<class...> class Metastruct>
struct MakeMetastruct
{
  template<typename... T>
  constexpr auto operator()(T...) const
  {
    return Metastruct<T...>{};
  }
};

template<template<typename...> class Metastruct>
constexpr MakeMetastruct<Metastruct> makeMetastruct{};

}//mpdef
#endif
