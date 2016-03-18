//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UID_HPP
#define NBDL_UID_HPP

namespace nbdl {
  // TODO make a factory for this from the old MsgUid stuff
  struct uid {
    char value[16];
  };
} // nbdl
#endif
