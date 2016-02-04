//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include<def/builder.hpp>
#include<def/directives.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/EnumerateProviders.hpp>
#include<Context.hpp>

namespace nbdl_def {
namespace builder {

/*
 * TODO build:
 *  ProviderMap
 *  StoreMap
 *  hardcode ListenerHandler for now
 *
 */

struct Context
{
  template<typename Def>
  constexpr auto operator()(Def)
  {
    constexpr Def def{};
    constexpr auto providersMeta = builder::enumerateProviders(def);
  } 
};
constexpr Context context{};

template<typename Context_>
struct ContextFactory
{
  template<typename... Args>
  auto operator()(Args... args)
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
