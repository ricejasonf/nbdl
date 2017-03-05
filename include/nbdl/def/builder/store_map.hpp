//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_STORE_MAP_HPP
#define NBDL_DEF_BUILDER_STORE_MAP_HPP

#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/make_store.hpp>

#include <boost/hana.hpp>

namespace nbdl_def::builder
{
  namespace hana = boost::hana;

  namespace details
  {

    struct build_store_pair_fn
    {

      template<typename AccessPoint>
      constexpr auto operator()(AccessPoint a)
      {
        using StoreTag = typename decltype(access_point_meta::store(a))::type;
        using PathType = decltype(access_point_meta::path(a));
        using EntityType = decltype(hana::back(access_point_meta::entities(a)));
        return hana::make_pair(
          PathType{},
          nbdl::make_store<StoreTag>(PathType{}, EntityType{})
        );
      }
    };
  }//details

  struct store_map_fn
  {
    template<typename AccessPoints>
    constexpr auto operator()(AccessPoints access_points) const
    {
      return decltype(hana::decltype_(hana::unpack(access_points,
        hana::make_map ^hana::on^ details::build_store_pair_fn{}))){};
    }
  };
  constexpr store_map_fn store_map{};
}
#endif
