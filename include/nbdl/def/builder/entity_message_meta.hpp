//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_MESSAGE_PAYLOAD_META_HPP
#define NBDL_DEF_MESSAGE_PAYLOAD_META_HPP

#include <mpdef/MPDEF_METASTRUCT.hpp>

namespace nbdl_def {
namespace builder {

MPDEF_METASTRUCT(
  entity_message_meta
  , path
  , entity_type 
  , private_payload
  , action
  , channel
);

}//builder
}//nbdl_def
#endif
