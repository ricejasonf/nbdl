//
// Copyright Jason Rice 2015-2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY

#include <mpdef/list.hpp>
#include <nbdl/macros/NBDL_MEMBERS.hpp>
#include <nbdl/macros/NBDL_MEMBER_NAMES.hpp>
#include <nbdl/macros/NBDL_MEMBER_IDS.hpp>

#define NBDL_ENTITY_NAME(RECORD) \
template <> struct entity_name_impl<RECORD> { \
  static auto apply() { return BOOST_HANA_STRING(#RECORD); }; \
};

#define NBDL_ENTITY(RECORD, ...) \
NBDL_ENTITY_NAME(RECORD) \
template <> struct entity_members_impl<RECORD> \
{ using type = mpdef::list<NBDL_MEMBERS(RECORD, __VA_ARGS__)>; }; \
NBDL_MEMBER_NAMES(RECORD, __VA_ARGS__)

#define NBDL_EMPTY(RECORD) \
template<> struct entity_members_impl<RECORD> \
{ using type = mpdef::list<>; };

#endif
