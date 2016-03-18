//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PATH_HPP
#define NBDL_DEF_BUILDER_PATH_HPP

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace details {

struct make_path_pair_from_entity_meta_fn
{
  template<typename T>
  constexpr auto operator()(T entity) const
  {
    return hana::make_pair(
      entity_key_meta::entity(entity_meta::key_meta(entity)),
      entity_key_meta::key(entity_meta::key_meta(entity))
    );
  }
};

}//details

struct path_fn
{
  template<typename EntityMap, typename AccessPoint>
  constexpr auto operator()(EntityMap entities, AccessPoint access_point) const
  {
    return hana::unpack(
      hana::transform(
        access_point_meta::entity_names(access_point),
        hana::partial(hana::at_key, entities)
      ),
      nbdl::make_path_type_from_pairs ^hana::on^ details::make_path_pair_from_entity_meta_fn{}
    );
  }
};
constexpr path_fn path{};

}//builder
}//nbdl_def
#endif
