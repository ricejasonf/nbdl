//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_WEBUI_DETAIL_DOM_MANIPS_HPP
#define NBDL_FWD_WEBUI_DETAIL_DOM_MANIPS_HPP

namespace nbdl::webui::detail
{
  struct begin { };
  struct end   { };

  template <typename ...>
  struct action_fn;

  // mut_action_fn should always have params <Tag, Store, ...>
  template <typename ...>
  struct mut_action_fn;
}
#endif
