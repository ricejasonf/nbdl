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

MPDEF_DIRECTIVE_LEAF(Type)
MPDEF_DIRECTIVE_LEAF(Template)
MPDEF_DIRECTIVE_LEAF(Name)

MPDEF_DIRECTIVE(Context)
MPDEF_DIRECTIVE(EndpointContext)
MPDEF_DIRECTIVE_LIST(Clients)
MPDEF_DIRECTIVE(Client)
MPDEF_DIRECTIVE(Api)
MPDEF_DIRECTIVE(Entities)
MPDEF_DIRECTIVE(ImportApi)
MPDEF_DIRECTIVE(ImportExternalApi)
MPDEF_DIRECTIVE_LIST(AccessPoints)
MPDEF_DIRECTIVE(AccessPoint)
MPDEF_DIRECTIVE(PrimaryKey)
MPDEF_DIRECTIVE(Path)
MPDEF_DIRECTIVE(Actions)
MPDEF_DIRECTIVE(Create)
MPDEF_DIRECTIVE(Read)
MPDEF_DIRECTIVE(Update)
MPDEF_DIRECTIVE(Delete)

MPDEF_DIRECTIVE_LEAF(Store)
MPDEF_DIRECTIVE_LEAF(StoreEmitter)

namespace tag
{
  struct PrimaryKeys_t {};
  constexpr auto PrimaryKeys = boost::hana::type_c<PrimaryKeys_t>;
}

}//nbdl_def

#endif
