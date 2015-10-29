//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_CONTEXT_DEFINITION_HPP
#define NBDL_DEF_CONTEXT_DEFINITION_HPP

#include "directives.hpp"
#include "../Store.hpp"

namespace nbdl_ddl {
namespace details {

template<typename ContextDef, typename Names>
constexpr auto entitiesFromNames(ContextDef ctx, Names names)
{
}

template<typename AccessPointDef>
constexpr auto pathType(AccessPointDef access_point)
{
  constexpr auto path_def = *meta::findByTag(access_point, tag::Path);
  //todo actually use a path definition instead of embedding the type
  return path_def;
}

template<typename ContextDef>
constexpr auto listenerHandler(ContextDef ctx)
{
  constexpr types = hana::make_map(
    hana::make_pair(tag::EndpointContext, hana::type_t<nbdl::ListenerHandlerDummy>),
    hana::make_pair(tag::Context, hana::type_t<nbdl::ListenerHandlerDummy>)
    //todo create listener handler type for servers ^
  );
  return types[hana::at(ctx, 0)];
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto storeName(ContextDef ctx, AccessPointDef access_point)
{
  constexpr auto store_def = hana::find_if(
    hana::make_tuple(
      meta::findByTag(ctx, tag::DefaultStore),
      meta::findByTag(access_point, tag::Store)
    ),
    hana::not_equal.to(hana::nothing)
  );
  return hana::maybe(BOOST_HANA_STRING("HashMap"), hana::second, store_def);
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto storeImpl(ContextDef ctx, AccessPointDef access_point)
{
  constexpr auto store_name = storeName(ctx, access_point);
  constexpr types = hana::make_map(
    hana::make_pair(BOOST_HANA_STRING("HashMap"), hana::type_t<nbdl::store_impl::HashMap>)
  );
  return types[hana::at(store_name, 1)];
}

template<typename ContextDef>
constexpr auto storeEmitterImpl(ContextDef ctx)
{
  constexpr auto store_name = hana::maybe(BOOST_HANA_STRING("HashMap"), hana::second,
    meta::findByTag(ctx, tag::StoreEmitter));
  constexpr types = hana::make_map(
    hana::make_pair(BOOST_HANA_STRING("HashMap"), hana::type_t<nbdl::store_impl::HashMap>)
  );
  return types[hana::second(store_name)];
}

template<typename ContextDef, typename AccessPointDef>
auto createStore(ContextDef ctx, AccessPointDef access_point)
{
  constexpr store_def = *hana::find_if(
    hana::make_tuple(
      hana::findByTag(ctx, tag::DefaultStore),
      hana::findByTag(access_point, tag::Store)
    ),
    hana::not_equal.to(hana::nothing)
  );
  using StoreImpl_ = decltype(storeImpl(store_def))::type;
  using StoreEmitterImpl_ = decltype(storeEmitterImpl(store_def))::type;
  using ListenerHandler_ = decltype(listenerHandler(ctx))::type;
  using Path_ = decltype(pathType(access_point))::type;
}

template<typename ContextDef>
auto storeMap(ContextDef ctx)
{
  return hana::transform(
    getByTag(
      *findByTag(ctx, tag::Api), 
      tag::AccessPoint),
    [](auto access_point) {
      auto path = *findByTag(access_point, tag::Path);
      return hana::make_pair(path, Store<ContextType, decltype(path)::type>{});
    });
}


}//details
}//nbdl_def

#endif
