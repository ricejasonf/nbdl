//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PROVIDER_MAP_HPP
#define NBDL_DEF_BUILDER_PROVIDER_MAP_HPP

#include <def/builder/Path.hpp>
#include <def/builder/ProviderMeta.hpp>
#include <mpdef/pair.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_def {
namespace builder {

namespace details {

  template<typename EntityMap>
  struct build_provider_pair_fn
  {
    template<typename ProviderMeta>
    constexpr auto operator()(ProviderMeta)
    {
      constexpr ProviderMeta provider_meta{};
      auto path_types = hana::unique(hana::transform(provider_meta::access_points(provider_meta),
        hana::partial(builder::path, EntityMap{})));
      return mpdef::make_pair(
        hana::append(path_types, provider_meta::name(provider_meta)),
        provider_meta::provider(provider_meta)
      );
    }
  };

}//details

struct provider_map_fn
{
  template<typename EntityMap, typename Providers>
  constexpr auto operator()(EntityMap, Providers providers) const
  {
    return hana::unpack(providers,
      hana::on(
        mpdef::make_map,
        details::build_provider_pair_fn<EntityMap>{}
      )
    );
  }
};
constexpr provider_map_fn provider_map{};

}//builder
}//nbdl_def
#endif
