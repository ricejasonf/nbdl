//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_STORE_MAP_HPP
#define NBDL_DEF_BUILDER_STORE_MAP_HPP

#include<def/builder/Path.hpp>
#include<def/builder/Store.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace details {

  template<typename EntityMap>
  struct BuildStorePair
  {
    EntityMap const& entity_map;

    template<typename AccessPoint>
    constexpr auto operator()(AccessPoint access_point)
    {
      auto path = builder::path(entity_map, access_point);
      return hana::make_pair(path, 
        typename decltype(builder::store(path, access_point))::type{});
    }
  };
  //could get rid of this because EntityMap **should** be default constructible
  template<typename EntityMap>
  constexpr BuildStorePair<EntityMap> buildStorePair{};

}//details

struct StoreMap
{
  template<typename EntityMap, typename AccessPoints>
  constexpr auto operator()(EntityMap entity_map, AccessPoints access_points) const
  {
    return decltype(hana::decltype_(hana::unpack(access_points,
      hana::make_map ^hana::on^ details::BuildStorePair<EntityMap>{entity_map}))){};
  }
};
constexpr StoreMap storeMap{};

}//builder
}//nbdl_def
#endif
