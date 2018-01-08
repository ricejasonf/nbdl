//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_UI_SPEC_HPP
#define NBDL_FWD_UI_SPEC_HPP

#include <mpdef/MPDEF_DIRECTIVE.hpp>

/*
 * nbdl::ui_spec - tools for impls like webui to manipulate paths for Store access
 */
namespace nbdl::ui_spec
{
  MPDEF_DIRECTIVE_LIST(concat);

  struct noop_t { }; // used implicitly in otherwise() et al

  struct path_tag { };

  template <typename ...>
  struct path_t
  { };

  template <typename ...>
  struct get_t
  { };

  struct key_at_tag { };

  template <typename PathSpec>
  struct key_at_t
  { };

  template <typename T, typename ...P>
  struct match_type { };
}

#endif
