//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_DIRECTIVES_HPP
#define NBDL_DEF_DIRECTIVES_HPP

#include "meta.hpp"

namespace nbdl_ddl {

namespace meta = nbdl_ddl_meta;

NBDL_DDL_DIRECTIVE(Type)
NBDL_DDL_DIRECTIVE(ListItem)

NBDL_DDL_DIRECTIVE(Context)
NBDL_DDL_DIRECTIVE(EndpointContext)
NBDL_DDL_DIRECTIVE(Api)
NBDL_DDL_DIRECTIVE(Entities)
NBDL_DDL_DIRECTIVE(ImportApi)
NBDL_DDL_DIRECTIVE(ImportExternalApi)
NBDL_DDL_DIRECTIVE(AccessPoint)
NBDL_DDL_DIRECTIVE(Path)
NBDL_DDL_DIRECTIVE(Actions)

}//nbdl_def

#endif
