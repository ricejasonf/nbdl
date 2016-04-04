//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include <mpdef/List.hpp>
#include <nbdl/context.hpp>
#include <def/directives.hpp>
#include <def/builder/ConsumerMap.hpp>
#include <def/builder/ContextCells.hpp>
#include <def/builder/EnumerateProviders.hpp>
#include <def/builder/EnumerateConsumers.hpp>
#include <def/builder/MapEntityMeta.hpp>
#include <def/builder/Path.hpp>
#include <def/builder/ProviderMap.hpp>
#include <def/builder/ProviderMeta.hpp>
#include <def/builder/StoreMap.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

struct context_fn
{
  template<typename Def>
  constexpr auto operator()(Def) const
  {
    static_assert(hana::first(Def{}) == tag::Context, "");
    constexpr auto defs = hana::second(Def{});

    constexpr auto entity_meta_map  = builder::map_entity_meta(defs[tag::Entities]);
    constexpr auto providers_meta   = builder::enumerate_providers(Def{});
    constexpr auto consumers_meta   = builder::enumerate_consumers(Def{});
    constexpr auto provider_map     = builder::provider_map(entity_meta_map, providers_meta);
    constexpr auto consumer_map     = builder::consumer_map(consumers_meta);
    constexpr auto cell_info        = builder::context_cells(provider_map, consumer_map);
    constexpr auto store_map        = builder::store_map(entity_meta_map,
                                        hana::flatten(
                                          hana::unpack(providers_meta,
                                          mpdef::make_list ^hana::on^ provider_meta::access_points)
                                        )
                                      );
    constexpr auto params = hana::append(cell_info, store_map);

    return hana::unpack(params, hana::template_<nbdl::context>);
  }
};
constexpr context_fn context{};

}//builder

template<typename ContextDef>
using make_context_t = typename decltype(builder::context(ContextDef{}))::type;

template<typename Context_>
struct context_factory_fn
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
  return context_factory_fn<Context_>{};
}

}//nbdl_def

#endif
