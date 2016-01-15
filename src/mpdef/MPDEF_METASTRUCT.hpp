//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MPDEF_METASTRUCT

#include<mpdef/details/_MPDEF_METASTRUCT_TEMPLATE_PARAMS.hpp>
#include<mpdef/details/_MPDEF_METASTRUCT_ACCESSORS.hpp>

#define MPDEF_METASTRUCT(NAME, ...) \
template<_MPDEF_METASTRUCT_TEMPLATE_PARAMS(__VA_ARGS__)> \
struct NAME { \
  _MPDEF_METASTRUCT_ACCESSORS(__VA_ARGS__) \
};

#endif
