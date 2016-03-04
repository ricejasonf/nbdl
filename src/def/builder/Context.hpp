//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include<mpdef/List.hpp>
#include<def/directives.hpp>
#include<def/builder/ConsumerMap.hpp>
#include<def/builder/ContextCells.hpp>
#include<def/builder/EnumerateProviders.hpp>
#include<def/builder/EnumerateConsumers.hpp>
#include<def/builder/MapEntityMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/ProviderMap.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<def/builder/StoreMap.hpp>
#include<Context.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

struct Context
{
  template<typename Def>
  constexpr auto operator()(Def) const
  {
    static_assert(hana::first(Def{}) == tag::Context, "");
    constexpr auto defs = hana::second(Def{});

    constexpr auto entity_meta_map  = builder::mapEntityMeta(defs[tag::Entities]);
    constexpr auto providers_meta   = builder::enumerateProviders(Def{});
    constexpr auto consumers_meta   = builder::enumerateConsumers(Def{});
    constexpr auto provider_map     = builder::providerMap(entity_meta_map, providers_meta);
    constexpr auto consumer_map     = builder::consumerMap(consumers_meta);
    constexpr auto cell_info        = builder::contextCells(provider_map, consumer_map);
    constexpr auto store_map        = builder::storeMap(entity_meta_map,
                                        hana::flatten(
                                          hana::unpack(providers_meta,
                                          mpdef::make_list ^hana::on^ ProviderMeta::accessPoints)
                                        )
                                      );
    constexpr auto params = hana::append(cell_info, store_map);

    return hana::unpack(params, hana::template_<nbdl::Context>);
  }
};
constexpr Context context{};

}//builder

template<typename ContextDef>
using make_context_t = typename decltype(builder::context(ContextDef{}))::type;

template<typename Context_>
struct ContextFactory
{
  template<typename ...Args>
  constexpr auto operator()(Args&& ...args) const
  {
    return Context_(std::forward<Args>(args)...);
  }
};

template<typename ContextDef>
constexpr auto make_context_factory(ContextDef const&)
{
  using Context_ = make_context_t<ContextDef>;
  return ContextFactory<Context_>{};
}

}//nbdl_def

#endif
