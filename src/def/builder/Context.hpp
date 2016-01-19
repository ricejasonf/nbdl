//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include<type_traits>
#include "../builder.hpp"
#include "../directives.hpp"
#include "./Path.hpp"
#include "./AccessPoint.hpp"
#include "../../Store.hpp"
#include "../../StoreCollection.hpp"
#include "../../Listener.hpp"
#include "../../Context.hpp"

namespace nbdl_def {
namespace builder {

template<typename ContextDef>
constexpr auto client(ContextDef ctx)
{
  auto client_def = *meta::findByTag(ctx, tag::Client);
  return hana::at(client_def, hana::int_c<0>);
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
      hana::type_c<nbdl::store_emitter::HashMap<ListenerHandler_, Path_>>)
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

template<typename DefPath>
class Context
{
  static constexpr auto def = hana::at_c<0>(DefPath{});

  constexpr auto listenerHandler()
  {
    return hana::type_c<nbdl::ListenerHandlerDummy<>>;
  }

  //todo put this in builder::AccessPoint
  template<typename AccessPoint>
  constexpr auto store(AccessPoint)
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

  constexpr auto accessPoints()
  {
    constexpr auto append_if = hana::reverse_partial(hana::reverse_partial(hana::if_, hana::id), hana::append);
    constexpr auto settings = mpdef::withSettings(tag::Store, tag::StoreEmitter);
    auto pred = hana::compose(hana::equal.to(tag::AccessPoint), hana::first);
    auto collector = mpdef::composeCollector(
      (
        mpdef::collectSettings,
        hana::demux(hana::if_)
        (hana::arg<2>, hana::arg<1>, 
          

        if (pred(x))
          append(state, x[tag::PrimaryKey]
      )
    );
  }

  auto storeMap()
  {
    //create store for every client/api access_point that has any action
    return hana::unpack(accessPoints(),
      [](auto... access_point) {
        return hana::make_tuple(hana::make_pair(
          access_point.buildPath(),
          access_point.buildStore())...);
      });
  }

  constexpr auto storeCollection()
  {
    using StoreMap_ = decltype(storeMap());
    using ListenerHandler_ = typename decltype(listenerHandler())::type;
    return hana::type_c<nbdl::StoreCollection<
      StoreMap_,
      ListenerHandler_
    >>;
  }

  public:

  using Tag = tag::AccessPoint_t;

  constexpr auto build()
  {
    //todo Client should have its own builder too
    using Traits = nbdl::details::ContextTraits<
      typename decltype(*meta::findSetting(DefPath{}, tag::Client))::type,
      void, //server
      typename decltype(listenerHandler())::type,
      typename decltype(storeCollection())::type
    >;

    return hana::type_c<nbdl::Context<Traits>>;
  }
};

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
