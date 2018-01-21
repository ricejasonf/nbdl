//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TAGS_HPP
#define NBDL_TAGS_HPP

namespace nbdl
{
  // Tags used for possible store values

  //
  //  trigger_read  - nbdl::context and others can trigger a read action
  //                  when a contained store matches this tag.
  //
  struct trigger_read { };

  //
  //  not_found     - Used to indicate that a resource does not exist
  //                  as the result of a read action
  //
  struct not_found { };

  //
  //  not_in_set    - AssociativeContainers can yield this tag when a key
  //                  is not in the set and not read action is desired.
  //
  struct not_in_set { };

  //
  //  unresolved    - A placeholder tag indicating that the final value is
  //                  pending the result of a read action.
  //                  (Also the default for nbdl::variant)
  //
  struct unresolved { };
}

#endif
