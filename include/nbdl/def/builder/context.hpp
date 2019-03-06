//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include <mpdef/list.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/def/builder/consumer_map.hpp>
#include <nbdl/def/builder/context_actors.hpp>
#include <nbdl/def/builder/enumerate_producers.hpp>
#include <nbdl/def/builder/enumerate_consumers.hpp>
#include <nbdl/def/builder/listener_lookup.hpp>
#include <nbdl/def/builder/message_api.hpp>
#include <nbdl/def/builder/producer_map.hpp>
#include <nbdl/def/builder/producer_meta.hpp>
#include <nbdl/def/builder/store_map.hpp>
#include <nbdl/make_def.hpp>

#include <boost/hana/concat.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/flatten.hpp>
#include <boost/hana/functional/on.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <utility>

namespace nbdl_def::builder
{

  namespace hana  = boost::hana;
  namespace hanax = boost::hana::experimental;

  struct make_actor_names_fn
  {
    // ProducerKeys is a list of lists of keys where each list
    // has the "name" in the first position
    template<typename ...ProducerMeta, typename ...ConsumerMeta>
    constexpr auto operator()(mpdef::list<ProducerMeta...>,
                              mpdef::list<ConsumerMeta...>) const

    {
      return hana::type_c<mpdef::list<
        decltype(producer_meta::name(ProducerMeta{}))...
      , decltype(consumer_meta::name(ConsumerMeta{}))...
      >>;
    }
  } constexpr make_actor_names{};



  template <
    typename ProducerLookup,
    typename ActorTagTypes,
    typename ActorNames,
    typename StoreMap,
    typename MessageApiMeta,
    typename ListenerLookup
  >
  struct context_meta
  {
    using producer_lookup       = ProducerLookup;
    using actor_tag_types       = ActorTagTypes;
    using actor_names           = ActorNames;
    using store_map             = StoreMap;
    using message_api_meta      = MessageApiMeta;
    using listener_lookup       = ListenerLookup;
  };

  struct context_fn
  {
    template <typename TagType>
    constexpr auto operator()(TagType) const
    {
      using Def = decltype(::nbdl::make_def(TagType{}));

      static_assert(hana::first(Def{}) == tag::Context, "");

      constexpr auto producers_meta   = builder::enumerate_producers(Def{});
      constexpr auto consumers_meta   = builder::enumerate_consumers(Def{});
      constexpr auto producer_map     = builder::producer_map(producers_meta);
      constexpr auto consumer_map     = builder::consumer_map(consumers_meta);
      constexpr auto actor_info       = builder::context_actors(producer_map,
                                                                consumer_map);
      constexpr auto actor_names      = builder::make_actor_names(
                                                                producers_meta,
                                                                consumers_meta);
      constexpr auto store_map        = builder::store_map(
                                          hana::flatten(
                                            hana::unpack(
                                              producers_meta
                                            , hana::on(
                                                mpdef::make_list
                                              , producer_meta::access_points
                                              ))));
      constexpr auto listener_lookup  = hana::type_c<
                                          builder::listener_lookup<
                                            decltype(producers_meta)
                                          >>;
      constexpr auto message_api      = hana::typeid_(
                                          builder::make_message_api(
                                            producers_meta
                                          ));
      constexpr auto params = hana::flatten(mpdef::make_list(
        actor_info,
        mpdef::make_list(actor_names),
        mpdef::make_list(store_map, message_api, listener_lookup)
      ));

      return hana::unpack(params, hana::template_<context_meta>);
    }
  };

  constexpr context_fn context{};

  template <typename Tag>
  using make_context_meta_t = typename decltype(builder::context(hana::type_c<Tag>))::type;
}

#endif
