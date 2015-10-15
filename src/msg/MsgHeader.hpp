//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MSG_HEADER_HPP
#define NBDL_MSG_HEADER_HPP

#include "../Variant.hpp"

namespace nbdl {

template<typename PathTypes>
struct MsgHeader
{
  using ActionType = Variant<Create, Read, Update, Delete>;
  ActionType action_type;

  int actionTypeId()
  {
    return action_type.match(
      [](Create) {
        return 1;
      },
      [](Read) {
        return 2;
      },
      [](Update) {
        return 3;
      },
      [](Delete) {
        return 4;
      },
      []() {
        return 0;
      });
  }
};

}//nbdl
