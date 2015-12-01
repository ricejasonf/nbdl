//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ENTITY_HPP
#define NBDL_DEF_ENTITY_HPP

#include "def.hpp"

namespace nbdl_def {
namespace def {

/*
 * TODO: Maybe replace the use of the following template specializations
 * that were used to define entities for serialization, validation,
 * and other stuff:
 *
 * EntityTraits
 * MemberId (integral identifier for serialization)
 * MemberName
 * Member* (pretty much all that member definition stuff)
 * GetPrimaryKey
 * GetForeignKey (relation stuff that can wait)
 *
 * note: I would abandon this if it were to increase the size
 * of the release target
 *
 */

}//def
}//nbdl_def

#endif
