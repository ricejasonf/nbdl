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
#include "../Path.hpp"
#include "../StoreCollection.hpp"
#include "../Listener.hpp"
#include "../Context.hpp"

namespace nbdl_ddl {
namespace hana = boost::hana;
namespace def {

template<typename ContextDef>
constexpr auto client(ContextDef ctx)
{
  auto client_def = *meta::findByTag(ctx, tag::Client);
  return hana::at(client_def, hana::int_c<0>);
}

template<typename AccessPointDef>
constexpr auto path(AccessPointDef access_point)
{
  auto path_def = *meta::findByTag(access_point, tag::Path);
  //todo actually use a path definition instead of embedding the type
  return hana::at(path_def, hana::int_c<0>);
}

template<typename Uhhh>
constexpr auto listenerHandler(Uhhh)
{
  /*
  auto types = hana::make_map(
    hana::make_pair(tag::EndpointContext, hana::type_c<nbdl::ListenerHandlerDummy<>>),
    hana::make_pair(tag::Context, hana::type_c<nbdl::ListenerHandlerDummy<>>)
    //todo create listener handler type for servers ^
  );
  return types[ctx_key];
  */
  return hana::type_c<nbdl::ListenerHandlerDummy<>>;
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto storeName(ContextDef ctx, AccessPointDef access_point)
{
  auto store_def = hana::find_if(
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
  using Path_ = typename decltype(path(access_point))::type;

  auto store_name = storeName(ctx, access_point);
  auto types = hana::make_map(
    hana::make_pair(BOOST_HANA_STRING("HashMap"),
      hana::type_c<nbdl::store::HashMap<Path_>>)
  );
  //return types[hana::at(store_name, hana::int_c<1>)];
  return types[store_name];
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto storeEmitterImpl(ContextDef ctx, AccessPointDef access_point)
{
  using ListenerHandler_ = typename decltype(listenerHandler(ctx))::type;
  using Path_ = typename decltype(path(access_point))::type;

  auto name = hana::maybe(BOOST_HANA_STRING("HashMap"), hana::second,
    meta::findByTag(ctx, tag::StoreEmitter));
  auto types = hana::make_map(
    hana::make_pair(BOOST_HANA_STRING("HashMap"),
      hana::type_c<nbdl::store_emitter::HashMap<Path_, ListenerHandler_>>)
  );
  return types[name];
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto store(ContextDef ctx, AccessPointDef access_point)
{
  using StoreImpl_ = typename decltype(storeImpl(ctx, access_point))::type;
  using StoreEmitterImpl_ = typename decltype(storeEmitterImpl(ctx, access_point))::type;
  using ListenerHandler_ = typename decltype(listenerHandler(ctx))::type;
  using Path_ = typename decltype(path(access_point))::type;
  return hana::type_c<nbdl::Store<
    StoreImpl_,
    StoreEmitterImpl_,
    ListenerHandler_,
    Path_ >>;
}

template<typename ContextDef>
auto storeMap(ContextDef ctx)
{
  return hana::fold_left(
    meta::filterByTag(
      *meta::findByTag(ctx, tag::Api), 
      tag::AccessPoint),
    hana::make_map(),
    [](auto state, auto access_point) {
      auto path = *meta::findByTag(access_point, tag::Path);
      hana::insert(state, hana::make_pair(path,
        typename decltype(store(ctx, access_point))::type{}));
    });
}

template<typename ContextDef>
constexpr auto storeCollection(ContextDef ctx)
{
  using StoreMap_ = decltype(storeMap(ctx));
  using ListenerHandler_ = typename decltype(listenerHandler(ctx))::type;
  return hana::type_c<nbdl::StoreCollection<
    StoreMap_,
    ListenerHandler_
  >>;
}

template<typename ContextDefPair>
constexpr auto context(ContextDefPair ctx_)
{
  auto ctx = hana::second(ctx_);
  auto traits = hana::make_tuple(
    hana::make_pair(BOOST_HANA_STRING("ListenerHandler_"), listenerHandler(hana::first(ctx_))),
    hana::make_pair(BOOST_HANA_STRING("StoreCollection_"), storeCollection(ctx)),
    hana::make_pair(BOOST_HANA_STRING("Client_"), client(ctx))
  );
  //promote nbdl::Context to use traits instead of details
  return hana::type_c<nbdl::details::Context<decltype(traits)>>;
}

template<typename Context_>
struct ContextFactory
{
  template<typename... Args>
  auto operator()(Args... args)
  {
    return Context_(args...);
  }
};
}//def

template<typename ContextDef>
constexpr auto buildContextFactory(ContextDef ctx_def)
{
  using Context_ = typename decltype(def::context(ctx_def))::type;
  return def::ContextFactory<Context_>{};
}

}//nbdl_ddl

#endif
