//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_DIRECTIVES_HPP
#define NBDL_DEF_DIRECTIVES_HPP

#include "meta.hpp"

namespace nbdl_def {

MPDEF_DIRECTIVE(Type)
MPDEF_DIRECTIVE(ListItem)

MPDEF_DIRECTIVE(Context)
MPDEF_DIRECTIVE(EndpointContext)
MPDEF_DIRECTIVE(Client)
MPDEF_DIRECTIVE(DefaultStore)
MPDEF_DIRECTIVE(Api)
MPDEF_DIRECTIVE(Entities)
MPDEF_DIRECTIVE(ImportApi)
MPDEF_DIRECTIVE(ImportExternalApi)
MPDEF_DIRECTIVE(AccessPoint)
MPDEF_DIRECTIVE(Path)
MPDEF_DIRECTIVE(Actions)
MPDEF_DIRECTIVE(Create)
MPDEF_DIRECTIVE(Read)
MPDEF_DIRECTIVE(Update)
MPDEF_DIRECTIVE(Delete)
MPDEF_DIRECTIVE(Store)
MPDEF_DIRECTIVE(StoreEmitter)

}//nbdl_def

#endif
