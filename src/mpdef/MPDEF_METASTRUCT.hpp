//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MPDEF_METASTRUCT

#include<mpdef/Metastruct.hpp>
#include<mpdef/details/_MPDEF_METASTRUCT_KEYS.hpp>
#include<mpdef/details/_MPDEF_METASTRUCT_SPEC_NAMES.hpp>

#define MPDEF_METASTRUCT(NAME, ...) \
struct NAME : ::mpdef::Metastruct<NAME> {\
  _MPDEF_METASTRUCT_KEYS(__VA_ARGS__) \
  static constexpr auto spec = mpdef::make_list( \
  _MPDEF_METASTRUCT_SPEC_NAMES(__VA_ARGS__) \
  ); \
}; \
constexpr auto make##NAME = mpdef::makeMetastruct<NAME>; \
constexpr auto make##NAME##WithMap = mpdef::makeMetastructWithMap<NAME>;

#endif
