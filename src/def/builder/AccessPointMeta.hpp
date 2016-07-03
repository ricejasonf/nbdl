//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ACCESS_POINT_META_HPP
#define NBDL_DEF_ACCESS_POINT_META_HPP

#include<mpdef/MPDEF_METASTRUCT.hpp>
#include<mpdef/List.hpp>

namespace nbdl_def {
namespace builder {

MPDEF_METASTRUCT(
  access_point_meta
  , name
  , actions
  , store
  , entity_names
);

#if 0

  // I'm going to leave this here to show what the MPDEF_METASTRUCT macro does,
  // and make it easier to try out different representations.

struct AccessPointMeta
  : ::mpdef::Metastruct<AccessPointMeta>
{
  static constexpr std::size_t length   = 5;
  static constexpr auto name            = at_c< 0 >;
  static constexpr auto actions         = at_c< 1 >;
  static constexpr auto storeContainer  = at_c< 2 >;
  static constexpr auto entityNames     = at_c< 3 >;

  static constexpr auto spec = mpdef::make_list(name, actions, storeContainer, entityNames);
};
constexpr auto makeAccessPointMeta = mpdef::makeMetastruct<AccessPointMeta>;
constexpr auto makeAccessPointMetaWithMap = mpdef::makeMetastructWithMap<AccessPointMeta>;
#endif

} // builder
} // nbdl_def

#endif
