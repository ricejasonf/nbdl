//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_DIRECTIVES_HPP
#define NBDL_DEF_DIRECTIVES_HPP

#include<mpdef/MPDEF_DIRECTIVE.hpp>

namespace nbdl_def {

MPDEF_DIRECTIVE_LEAF(Type)
MPDEF_DIRECTIVE_LEAF(Name)

MPDEF_DIRECTIVE(Context)
MPDEF_DIRECTIVE_LIST(Providers)
MPDEF_DIRECTIVE(Provider)
MPDEF_DIRECTIVE_LIST(AccessPoints)
MPDEF_DIRECTIVE(AccessPoint)
MPDEF_DIRECTIVE(Actions)
MPDEF_DIRECTIVE(Create)
MPDEF_DIRECTIVE(Read)
MPDEF_DIRECTIVE(Update)
MPDEF_DIRECTIVE(UpdateRaw)
MPDEF_DIRECTIVE(Delete)

MPDEF_DIRECTIVE_LEAF(StoreContainer)
MPDEF_DIRECTIVE_LEAF(PrivatePayload)
MPDEF_DIRECTIVE_LEAF(UseLocalVersion)
MPDEF_DIRECTIVE_LEAF(FireAndForget)

// used as key to entity map
MPDEF_DIRECTIVE_LEAF(EntityName)

MPDEF_DIRECTIVE(Entities)
MPDEF_DIRECTIVE(Entity)
MPDEF_DIRECTIVE(PrimaryKey)

}//nbdl_def

#endif
