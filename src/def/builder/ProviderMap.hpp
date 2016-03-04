//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PROVIDER_MAP_HPP
#define NBDL_DEF_BUILDER_PROVIDER_MAP_HPP

#include<def/builder/Path.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<mpdef/Pair.hpp>
#include<ProviderMap.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace details {

  template<typename EntityMap>
  struct BuildProviderPair
  {
    template<typename ProviderMeta_>
    constexpr auto operator()(ProviderMeta_)
    {
      constexpr ProviderMeta_ provider_meta{};
      auto path_types = hana::transform(ProviderMeta::accessPoints(provider_meta),
        hana::partial(builder::path, EntityMap{}));
      return mpdef::make_pair(
        hana::append(path_types, ProviderMeta::name(provider_meta)),
        ProviderMeta::provider(provider_meta)
      );
    }
  };

}//details

struct ProviderMap
{
  template<typename EntityMap, typename Providers>
  constexpr auto operator()(EntityMap, Providers providers) const
  {
    return hana::unpack(providers,
      hana::on(
        mpdef::make_map,
        details::BuildProviderPair<EntityMap>{}
      )
    );
  }
};
constexpr ProviderMap providerMap{};

}//builder
}//nbdl_def
#endif
