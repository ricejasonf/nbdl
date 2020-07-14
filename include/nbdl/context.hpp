//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include <nbdl/actor_type.hpp>
#include <nbdl/apply_message.hpp>
#include <nbdl/concept/Consumer.hpp>
#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/concept/Producer.hpp>
#include <nbdl/concept/StateConsumer.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/consumer_init.hpp>
#include <nbdl/def/builder/context.hpp>
#include <nbdl/match.hpp>
#include <nbdl/message.hpp>
#include <nbdl/notify_state_change.hpp>
#include <nbdl/producer_init.hpp>
#include <nbdl/send_downstream_message.hpp>
#include <nbdl/send_upstream_message.hpp>
#include <nbdl/tags.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/hana/ext/std/integer_sequence.hpp>
#include <boost/mp11/list.hpp>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana  = boost::hana;

  namespace context_detail
  {
    struct store_tag { };

    constexpr auto init_actors = [](auto& actors)
    {
      hana::for_each(actors, [](auto& actor)
      {
        using Actor = std::decay_t<decltype(actor)>;

        if constexpr(nbdl::Producer<Actor>)
        {
          nbdl::producer_init(actor);
        }
      });
      hana::for_each(actors, [](auto& actor)
      {
        using Actor = std::decay_t<decltype(actor)>;

        if constexpr(nbdl::Consumer<Actor>)
        {
          nbdl::consumer_init(actor);
        }
      });
    };

    template <typename Context>
    class store_handle
    {
      Context& ctx;

      public:

      using tag = typename Context::tag;
      using hana_tag = context_detail::store_tag;
      friend apply_message_impl<hana_tag>;
      friend match_impl<hana_tag>;
      friend get_impl<hana_tag>;

      store_handle(Context& c) : ctx(c) { }
    };
  }

  struct context_tag { };

  template <typename Tag>
  class context
  {
  public:
    using tag = Tag;
  private:
    using ContextMeta     = nbdl_def::builder::make_context_meta_t<Tag>;
    using ProducerLookup  = typename ContextMeta::producer_lookup;
    using ActorTagTypes   = typename ContextMeta::actor_tag_types;
    using ActorNames      = typename ContextMeta::actor_names;
    using StoreMap        = typename ContextMeta::store_map;
    using ListenerLookup  = typename ContextMeta::listener_lookup;

    friend apply_message_impl<context_tag>;
    friend get_impl<context_tag>;
    friend match_impl<context_tag>;
    friend apply_message_impl<context_detail::store_tag>;
    friend match_impl<context_detail::store_tag>;

    using Actors = mp_apply<
      hana::tuple
    , mp_transform_q<
        mp_bind_back<nbdl::actor_type_t, context_detail::store_handle<context>>
      , ActorTagTypes
      >
    >;

    Actors actors;
    StoreMap stores;

    template <typename Message>
    void propagate_upstream(Message&& m)
    {
      using Index = decltype(
        hana::at_key(
          ProducerLookup{}
        , message::get_path_type(m)
        )
      );
      nbdl::send_upstream_message(actors[Index{}], std::forward<Message>(m));
    }

    template <typename Message>
    void propagate_downstream(Message const& m)
    {
      // notify all consumers
      hana::for_each(actors, [&](auto& actor)
      {
        if constexpr(nbdl::Consumer<decltype(actor)>)
        {
          nbdl::send_downstream_message(actor, m);
        }
      });
    }

    template <typename Path>
    void notify_state_consumers(Path const& p)
    {
      hana::for_each(actors, [&](auto& actor)
      {
        if constexpr(nbdl::StateConsumer<decltype(actor)>)
        {
          nbdl::notify_state_change(actor, p);
        }
      });
    }

    // All actions MUST be applied to the
    // store BEFORE the message is propagated!

    // called inside PushUpstreamMessageFn and PushDownstreamMessageFn
    template <typename Message>
    auto push_message(Message const& m)
      -> std::enable_if_t<message::is_path_normalizable<Message>>
    {
      constexpr auto path_type = decltype(message::get_path_type(m)){};
      auto& store = stores[path_type];
      if constexpr(message::is_upstream<Message> && message::is_read<Message>)
      {
        nbdl::apply_message(store, m);
        // send response message
        nbdl::match(store, message::get_path(m), hana::overload_linearly(
          [&](nbdl::trigger_read)
          {
            // The Store should create an 'unresolved'
            // placeholder to prevent duplicate upstream
            // read requests and so any StateConsumer
            // gets a value
            propagate_upstream(m);
          },
          [&](nbdl::unresolved)
          {
            // Do nothing. A read is already in progress.
            // The Consumer should notify the appropriate requestors
            // once the response (downstream read message)
            // is received.
          },
          [&](auto&& value)
          {
            // There is a resolved value in the store
            // so send it as a downstream read message,
            // and don't bother the Producer with it.
            propagate_downstream(
              message::to_downstream_read(m, std::forward<decltype(value)>(value))
            );
          }
        ));
      }
      else
      {
        if constexpr(!decltype(hana::equal(
          hana::type_c<decltype(nbdl::apply_message(store, m))>,
          hana::type_c<hana::false_>
        ))::value)
        {
          if (nbdl::apply_message(store, m))
          {
            // the state changed
            notify_state_consumers(message::get_path(m));
          }
        }

        if constexpr(message::is_upstream<Message>)
        {
          propagate_upstream(m);
        }
        else
        {
          propagate_downstream(m);
        }
      }

      // apply the action to stores that are listening to the path_type
      constexpr auto listeners = hana::find(ListenerLookup{}, path_type).value_or(mpdef::list<>{});
      hana::for_each(listeners, [&](auto listener_path_type)
      {
        nbdl::apply_foreign_message(stores[listener_path_type], m, [&](auto const& path)
        {
          static_assert(
            decltype(
              hana::equal(
                hana::typeid_(path)
              , listener_path_type
              )
            )::value
          , "Store can only mark its own path as modified."
          );

          notify_state_consumers(path);
        });
      });
    }
    
    template <typename Message>
    auto push_message(Message const& m)
      -> std::enable_if_t<!message::is_path_normalizable<Message> && message::is_upstream<Message>>
    {
      propagate_upstream(m);
    }

  public:
    template <typename ...Args>
    explicit context(Args&& ...args)
      : actors(detail::make_actor_initializer(
            context_detail::store_handle<context>(*this)
          , std::forward<Args>(args)
        )...)
      , stores()
    {
      context_detail::init_actors(actors);
    }

    // default constructor (deprecated - used in tests only I think)
    context()
      : actors(hana::cycle(
          hana::make_tuple(detail::make_actor_initializer(
            context_detail::store_handle<context>(*this)
          , hana::type_c<void>
          ))
        , decltype(hana::length(std::declval<Actors>())){}
        ))
      , stores()
    {
      context_detail::init_actors(actors);
    }

    context(context const&) = delete;

    // Access actor by its position
    // in the tuple of Producers/Consumers
    template <std::size_t i>
    auto& actor()
    {
      return hana::at_c<i>(actors);
    }
  };

  // Store

  template <>
  struct apply_message_impl<context_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m)
    {
      s.push_message(std::forward<Message>(m));
      return hana::true_c;
    }
  };

  template <>
  struct get_impl<context_tag>
  {
    template <typename Store, typename Key>
    static constexpr decltype(auto) apply(Store const& s, Key&& k)
    {
      constexpr auto path_type = decltype(hana::typeid_(k)){};
      return nbdl::get(s.stores[path_type], std::forward<Key>(k));
    }
  };

  template <>
  struct match_impl<context_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      constexpr auto path_type = decltype(hana::typeid_(k)){};
      nbdl::match(s.stores[path_type], k, [&](auto const& value)
      {
        if constexpr(
          decltype(hana::equal(hana::typeid_(value), hana::type_c<nbdl::trigger_read>)){}
        )
        {
          // Trigger upstream read request.
          // This is a special case that stores can use to trigger
          // read requests automatically.
          // The store's value should be set to `nbdl::unresolved`
          [&](auto d1, auto d2) // makes dependent
          {
            s.push_message(message::make_upstream_read(k, d1));
            std::forward<Fn>(fn)(d2);
          }(message::no_uid, nbdl::unresolved{});
        }
        else
        {
          // Users should not be allowed to modify values directly.
          std::forward<Fn>(fn)(value);
        }
      });
    }
  };

  // As a Store for StateConsumers

  template <>
  struct apply_message_impl<context_detail::store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m)
    {
      /* Removed assertion that only Producers
       * can send downstream messages because
       * it was to compile-time intensive.
      static_assert(
        nbdl::Producer<???>::value or not message::is_downstream<Message>
      , "Only Producers can send a downstream message."
      );
       */

      std::forward<Store>(s)
        .ctx.push_message(std::forward<Message>(m));
      return hana::true_c;
    }
  };

  template <>
  struct get_impl<context_detail::store_tag>
  {
    template <typename Store, typename Key>
    static constexpr decltype(auto) apply(Store const& s, Key&& k)
    {
      return nbdl::get(s.ctx, std::forward<Key>(k));
    }
  };

  template <>
  struct match_impl<context_detail::store_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      nbdl::match(std::forward<Store>(s).ctx,
                  std::forward<Key>(k),
                  std::forward<Fn>(fn));
    }
  };
}

namespace nbdl::ui_spec::detail
{
  template <typename Context, typename X, typename ...Xs>
  struct get_type_at_path_impl<context_detail::store_handle<Context>, path_t<get_t<X, Xs...>>>
  {
    using S = typename nbdl_def::builder::make_context_meta_t<typename Context::tag>::store_map;
    using type = decltype(
      nbdl::get_path(std::declval<S const&>()[
        hana::type_c<X>
      ], std::declval<hana::tuple<X, Xs...>>())
    );
  };
}

namespace boost::hana
{
  template <typename ...T>
  struct tag_of<nbdl::context<T...>>
  {
    using type = nbdl::context_tag;
  };
}

#endif
