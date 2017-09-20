//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_DIRECTIVES_HPP
#define NBDL_DEF_DIRECTIVES_HPP

#include <mpdef/MPDEF_DIRECTIVE.hpp>

namespace nbdl_def
{
  MPDEF_DIRECTIVE(AccessPoint)
  MPDEF_DIRECTIVE(Actions)
  MPDEF_DIRECTIVE(Consumer)
  MPDEF_DIRECTIVE(Context)
  MPDEF_DIRECTIVE(Create)
  MPDEF_DIRECTIVE(Delete)
  MPDEF_DIRECTIVE(Producer)
  MPDEF_DIRECTIVE(Read)
  MPDEF_DIRECTIVE(Update)
  MPDEF_DIRECTIVE_LEAF(Name)
  MPDEF_DIRECTIVE_LEAF(PrivatePayload)
  MPDEF_DIRECTIVE_LEAF(Type)
  MPDEF_DIRECTIVE_LIST(AccessPoints)
  MPDEF_DIRECTIVE_LIST(Consumers)
  MPDEF_DIRECTIVE_LIST(ListenPaths)
  MPDEF_DIRECTIVE_LIST(Producers)
  MPDEF_DIRECTIVE_TYPE(Entity)
  MPDEF_DIRECTIVE_TYPE(PathKey)
  MPDEF_DIRECTIVE_TYPE(Store)
  MPDEF_DIRECTIVE_TYPE_LIST(Path)
}

#endif
