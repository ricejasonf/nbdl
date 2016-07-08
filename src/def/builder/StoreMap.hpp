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
  struct build_store_pair_fn
  {

    template<typename AccessPoint>
    constexpr auto operator()(AccessPoint a)
    {
      using StoreTag = typename decltype(access_point_meta::store(a))::type;
      using PathType = decltype(builder::path(EntityMap{}, a));
      return hana::make_pair(
        PathType{},
        nbdl::make_store<StoreTag>(PathType{})
      );
    }
  };
  //could get rid of this because EntityMap **should** be default constructible
  template<typename EntityMap>
  constexpr build_store_pair_fn<EntityMap> build_store_pair{};

}//details

struct store_map_fn
{
  template<typename EntityMap, typename AccessPoints>
  constexpr auto operator()(EntityMap, AccessPoints access_points) const
  {
    return decltype(hana::decltype_(hana::unpack(access_points,
      hana::make_map ^hana::on^ details::build_store_pair<EntityMap>))){};
  }
};
constexpr store_map_fn store_map{};

}//builder
}//nbdl_def
#endif
