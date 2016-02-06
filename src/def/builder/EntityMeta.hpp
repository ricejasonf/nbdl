//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ENTITY_META_HPP
#define NBDL_DEF_ENTITY_META_HPP

#include<mpdef/MPDEF_METASTRUCT.hpp>
#include<mpdef/MakeMetastruct.hpp>

namespace nbdl_def {
namespace builder {

MPDEF_METASTRUCT(
  EntityMeta
  , keyMeta
  , membersMeta
);
constexpr auto makeEntityMeta = mpdef::makeMetastruct<EntityMeta>;

}//builder
}//nbdl_def
#endif
