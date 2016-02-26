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
#include<def/builder/MapEntityMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/ProviderMap.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<def/builder/EnumerateProviders.hpp>
#include<def/builder/StoreMap.hpp>
#include<Context.hpp>

namespace nbdl_def {
namespace builder {

struct Context
{
  template<typename Def>
  constexpr auto operator()(Def) const
  {
    static_assert(hana::first(Def{}) == tag::Context, "");
    constexpr auto defs = hana::second(Def{});
    constexpr auto entityMetaMap = builder::mapEntityMeta(defs[tag::Entities]);
    constexpr auto providersMeta = builder::enumerateProviders(Def{});
    return hana::template_<nbdl::Context>(
      hana::template_<nbdl::details::ContextTraits>(
        builder::providerMap(entityMetaMap, providersMeta),
        hana::type_c<void>, //Consumers
        builder::storeMap(entityMetaMap,
          hana::flatten(hana::unpack(providersMeta,
            mpdef::make_list ^hana::on^ ProviderMeta::accessPoints)))
      )
    );
  }
};
constexpr Context context{};

template<typename Context_>
struct ContextFactory
{
  template<typename... Args>
  auto operator()(Args... args) const
  {
    return Context_::create(args...);
  }
};

}//builder

template<typename ContextDef>
constexpr auto buildContextFactory(ContextDef ctx_def)
{
  using Context_ = typename decltype(builder::context(ctx_def))::type;
  return builder::ContextFactory<Context_>{};
}

}//nbdl_def

#endif
