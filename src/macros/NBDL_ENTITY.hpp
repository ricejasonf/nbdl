//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY

#include "NBDL_MEMBERS.hpp"
#include "NBDL_MEMBER_NAMES.hpp"
#include "NBDL_MEMBER_IDS.hpp"

#define NBDL_ENTITY(NAME, ...) \
template<> struct EntityTraits<NAME>								\
{ using Members = MemberSet<NBDL_MEMBERS(NAME, __VA_ARGS__)>; };	\
NBDL_MEMBER_NAMES(NAME, __VA_ARGS__) \
NBDL_MEMBER_IDS(NAME, __VA_ARGS__) 

#define NBDL_ENTITY_WITHOUT_MEMBER_IDS(NAME, ...) \
template<> struct EntityTraits<NAME>								\
{ using Members = MemberSet<NBDL_MEMBERS(NAME, __VA_ARGS__)>; };	\
NBDL_MEMBER_NAMES(NAME, __VA_ARGS__)

#endif
