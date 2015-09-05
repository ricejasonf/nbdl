//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ACTIONS_UPDATE
#define NBDL_ACTIONS_UPDATE

#include "msg/MsgId.hpp"

namespace nbdl {

template<typename Context, typename PathType>
class Update
{
	MsgId action_key;
	PathType path;
	//todo store the entity or a diff object or something
}

}//nbdl

#endif
