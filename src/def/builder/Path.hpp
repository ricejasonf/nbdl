//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PATH_HPP
#define NBDL_DEF_BUILDER_PATH_HPP

#include<def/builder.hpp>
#include<Path.hpp>

#include<type_traits>

namespace nbdl_def {
namespace builder {

namespace details {

struct MakePathPairFromEntityMeta
{
  template<typename T>
  constexpr auto operator()(T entity) const
  {
    return hana::make_pair(entity.entity(), entity.key());
  }
};
constexpr MakePathPairFromEntityMeta makePathPairFromEntityMeta{};

}//details

struct Path
{
  template<typename EntityMap, typename AccessPoint>
  constexpr auto operator()(EntityMap entities, AccessPoint access_point) const
  {
    return hana::unpack(
      hana::transform(access_point.entityNames(), hana::partial(hana::at_key, entities)),
      nbdl::makePathTypeFromPairs ^hana::on^ details::makePathPairFromEntityMeta
    );
  }
};
constexpr Path path{};

}//builder
}//nbdl_def
#endif
