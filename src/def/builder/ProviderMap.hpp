//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PROVIDER_MAP_HPP
#define NBDL_DEF_BUILDER_PROVIDER_MAP_HPP

#include<def/builder/Path.hpp>
#include<ProviderMap.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace details {

  template<typename EntityMap>
  struct BuildProviderPair
  {
    EntityMap const& entity_map;

    template<typename ProviderMeta>
    constexpr auto operator()(ProviderMeta)
    {
      constexpr ProviderMeta provider_meta{};
      auto path_types = hana::transform(provider_meta.accessPoints(),
        hana::partial(builder::path, entity_map));
      return decltype(hana::decltype_(
        hana::make_pair(hana::make_type(hana::append(path_types, provider_meta.name())), 
        std::declval<typename decltype(provider_meta.provider())::type>())
      )){};
    }
  };

}//details

struct ProviderMap
{
  template<typename EntityMap, typename Providers>
  constexpr auto operator()(EntityMap entity_map, Providers providers) const
  {
    return decltype(hana::decltype_(hana::unpack(providers,
      hana::template_<nbdl::ProviderMap> ^hana::on^ details::BuildProviderPair<EntityMap>{entity_map}))
    ){};
  }
};
constexpr ProviderMap providerMap{};

}//builder
}//nbdl_def
#endif