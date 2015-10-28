//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_CONTEXT_DEFINITION_HPP
#define NBDL_DEF_CONTEXT_DEFINITION_HPP

#include<boost/hana.hpp>

namespace nbdl_def {

//get first items from contained defs
struct pairFromDef_t
{
  template<typename KeyTag, typename ValueTag, typename Def>
  constexpr auto operator()(KeyTag key_tag, ValueTag value_tag, EntitiesDef def)
  {
    return hana::make_pair(
        hana::at(*hana::findByTag(def, key_tag), 1),
        hana::at(*hana::findByTag(def, value_tag), 1)
    );
  }
}
constexpr pairFromDef_t pairFromDef{};

template<typename KeyTag, typename ValueTag, typename EntitiesDef>
constexpr auto mapify(KeyTag key_tag, ValueTag value_tag, EntitiesDef def)
{
  return hana::fold_left(
    hana::drop_front(def),
    hana::make_map(),
    hana::insert ^hana::on^ hana::partial(pairFromDef, key_tag, value_tag)
  );
}
template<typename ContextDef, typename Names>
constexpr auto entitiesFromNames(ContextDef ctx, Names names)
{
}

template<typename AccessPointDef>
constexpr auto pathType(AccessPointDef access_point)
{
  constexpr auto path_def = *findByTag(access_point, tag::Path);
  return hana::type_t<nbdl::CreatePath>;
}
template<typename ContextDef>
constexpr auto listenerHandler(ContextDef ctx)
{
  constexpr types = hana::make_map(
    hana::make_pair(tag::EndPointContextDefinition, hana::type_t<nbdl::ListenerHandlerDummy>),
    hana::make_pair(tag::ContextDefinition, hana::type_t<nbdl::ListenerHandlerDummy>)
    //todo create listener handler type for servers ^
  );
  return types[hana::at(ctx, 0)];
}

template<typename ContextDef, typename AccessPointDef>
constexpr auto storeName(ContextDef ctx, AccessPointDef access_point)
{
  constexpr store_def = *hana::find_if(
    hana::make_tuple(
      hana::findByTag(ctx, tag::DefaultStore),
      hana::findByTag(access_point, tag::Store)
    ),
    hana::not_equal.to(hana::nothing)
  );
  return hana::at(store_def, 1);
}
template<typename ContextDef, typename AccessPointDef>
constexpr auto storeImpl(ContextDef ctx, AccessPointDef access_point)
{
  constexpr auto store_name = storeName(ctx, access_point);
  constexpr types = hana::make_map(
    hana::make_pair(BOOST_HANA_STRING("HashMap"), hana::type_t<nbdl::store_impl::HashMap>)
  );
  return types[hana::at(store_def, 1)];
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
  using ListenerHandler_ = decltype(listenerHandler(ctx))::type
}

template<typename ContextDef>
auto storeMap(ContextDef ctx)
{
  return hana::transform(
    getByTag(
      *findByTag(ctx, tag::ApiDefinition), 
      tag::AccessPoint),
    [](auto access_point) {
      auto path = *findByTag(access_point, tag::Path);
      return hana::make_pair(path, Store<ContextType, decltype(path)::type>{});
    });
}


}//nbdl_def

#endif
