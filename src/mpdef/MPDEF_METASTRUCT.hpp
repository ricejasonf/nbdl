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
struct NAME : ::mpdef::metastruct<NAME> {\
  _MPDEF_METASTRUCT_KEYS(__VA_ARGS__) \
  static constexpr auto spec = mpdef::make_list( \
  _MPDEF_METASTRUCT_SPEC_NAMES(__VA_ARGS__) \
  ); \
}; \
constexpr auto make_##NAME = mpdef::make_metastruct<NAME>; \
constexpr auto make_##NAME##_with_map = mpdef::make_metastruct_with_map<NAME>;

#endif
