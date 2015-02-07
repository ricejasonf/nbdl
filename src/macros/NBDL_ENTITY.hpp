#ifndef NBDL_ENTITY

#include "NBDL_MEMBERS.hpp"
#include "NBDL_MEMBER_NAMES.hpp"

#define NBDL_ENTITY(NAME, ...) \
template<> struct EntityTraits<NAME>								\
{ using Members = MemberSet<NBDL_MEMBERS(NAME, __VA_ARGS__)>; };	\
NBDL_MEMBER_NAMES(NAME, __VA_ARGS__)

#endif
