//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include <nbdl/apply_message.hpp>
#include <nbdl/concept/Consumer.hpp>
#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/concept/Producer.hpp>
#include <nbdl/concept/StateConsumer.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/def/builder/context.hpp>
#include <nbdl/detail/concept_pred.hpp>
#include <nbdl/detail/normalize_path_type.hpp>
#include <nbdl/match.hpp>
#include <nbdl/make_consumer.hpp>
#include <nbdl/make_producer.hpp>
#include <nbdl/make_state_consumer.hpp>
#include <nbdl/message.hpp>
#include <nbdl/producer_init.hpp>
#include <nbdl/send_downstream_message.hpp>
#include <nbdl/send_upstream_message.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/hana/ext/std/integer_sequence.hpp>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana  = boost::hana;
  namespace hanax = boost::hana::experimental;

  namespace context_detail
  {
    struct store_tag { };

    constexpr auto init_producers = [](auto& cells)
    {
      hana::for_each(cells, [](auto& cell)
      {
        using Cell = std::decay_t<decltype(cell)>;

        if constexpr(nbdl::Producer<Cell>::value)
        {
          nbdl::producer_init(cell);
        }
      });
    };
  }

  struct context_tag { };

  template <typename Tag, typename ArgTypes>
  class context
  {
    using tag = Tag;
    using ContextMeta     = nbdl_def::builder::make_context_meta_t<Tag>;
    using ProducerLookup  = typename ContextMeta::producer_lookup;
    using CellTagTypes    = typename ContextMeta::cell_tag_types;
    using StoreMap        = typename ContextMeta::store_map;
    using ListenerLookup  = typename ContextMeta::listener_lookup;

    friend apply_message_impl<context_tag>;
    friend get_impl<context_tag>;
    friend match_impl<context_tag>;
    friend apply_message_impl<context_detail::store_tag>;
    friend match_impl<context_detail::store_tag>;

    // calling the push functions after
    // the context is destroyed will result
    // in undefined behaviour
    class push_upstream_api
    {
      context& ctx;

      public:

      using tag = tag;

      push_upstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m) const
      {
        static_assert(nbdl::UpstreamMessage<Message>::value,
          "nbdl::context::push_upstream_api requires an UpstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }
    };

    class push_downstream_api
    {
      context& ctx;

      public:

      using tag = tag;

      push_downstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m) const
      {
        static_assert(nbdl::DownstreamMessage<Message>::value,
          "nbdl::context::push_downstream_api requires a DownstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }
    };

    class push_upstream_api_state_consumer
    {
      context& ctx;

      public:

      using tag = tag;
      using hana_tag = context_detail::store_tag;
      friend apply_message_impl<hana_tag>;
      friend match_impl<hana_tag>;
      friend get_impl<hana_tag>;

      push_upstream_api_state_consumer(context& c) : ctx(c) { }
    };

    static constexpr std::size_t cell_count = decltype(hana::size(CellTagTypes{}))::value;
    static constexpr auto cell_index_sequence = std::make_index_sequence<cell_count>{};

    template <std::size_t i>
    static constexpr auto cell_factory()
    {
      using CellTag = typename decltype(hana::at_c<i>(CellTagTypes{}))::type;
      if constexpr(nbdl::Producer<CellTag>::value)
        return nbdl::make_producer<CellTag>;
      else if constexpr(nbdl::StateConsumer<CellTag>::value)
        return nbdl::make_state_consumer<CellTag>;
      else // if Consumer
        return nbdl::make_consumer<CellTag>;
    }

    template <std::size_t i>
    static constexpr auto get_push_api_type()
    {
      using CellTag = typename decltype(hana::at_c<i>(CellTagTypes{}))::type;
      if constexpr(nbdl::Producer<CellTag>::value)
        return hana::type_c<push_downstream_api>;
      else if constexpr(nbdl::StateConsumer<CellTag>::value)
        return hana::type_c<push_upstream_api_state_consumer>;
      else // if Consumer
        return hana::type_c<push_upstream_api>;
    }

    template <std::size_t i>
    using PushApi = typename decltype(get_push_api_type<i>())::type;

    template <std::size_t i, typename Arg = hana::type<void>>
    static constexpr auto make_cell_type()
    {
      if constexpr(decltype(hana::equal(hana::typeid_(std::declval<Arg>()), hana::type_c<void>)){})
      {
        return hana::type_c<decltype(cell_factory<i>()(std::declval<PushApi<i>>()))>;
      }
      else
      {
        return hana::type_c<decltype(cell_factory<i>()(std::declval<PushApi<i>>(),
           std::declval<Arg>()))>;
      }
    }

    // For default construction.
    template < std::size_t ...i>
    static constexpr auto make_storage_helper(std::index_sequence<i...>, hanax::types<>)
    {
      return hana::type_c<decltype(hana::make_tuple(
        hana::traits::declval(make_cell_type<i>())...
      ))>;
    }

    // For single argument construction
    template <std::size_t i1, std::size_t ...i, typename Arg1, typename ...Args>
    static constexpr auto make_storage_helper(std::index_sequence<i1, i...>, hanax::types<Arg1, Args...>)
    {
      return hana::type_c<decltype(hana::make_tuple(
        hana::traits::declval(make_cell_type<i1, Arg1>()),
        hana::traits::declval(make_cell_type<i, Args>())...
      ))>;
    }

    using Cells = typename decltype(make_storage_helper(cell_index_sequence, ArgTypes{}))::type;

    Cells cells;
    StoreMap stores;

    template <std::size_t i>
    decltype(auto) make_cell()
    {
      return cell_factory<i>()(PushApi<i>(*this));
    }

    template <std::size_t i, typename Arg>
    decltype(auto) make_cell(Arg&& arg)
    {
      if constexpr(decltype(hana::equal(hana::typeid_(arg), hana::type_c<void>)){})
      {
        return cell_factory<i>()(PushApi<i>(*this));
      }
      else
      {
        return cell_factory<i>()(PushApi<i>(*this), std::forward<Arg>(arg));
      }
    }

    template <typename Message>
    void propagate_upstream(Message&& m)
    {
      using Index = decltype(
        hana::at_key(
          ProducerLookup{}
        , message::get_path_type(m)
        )
      );
      nbdl::send_upstream_message(cells[Index{}], std::forward<Message>(m));
    }

    template <typename Message>
    void propagate_downstream(Message const& m)
    {
      // notify all consumers
      hana::for_each(cells, [&](auto& cell)
      {
        if constexpr(nbdl::Consumer<decltype(cell)>::value)
        {
          nbdl::send_downstream_message(cell, m);
        }
      });
    }

    template <typename Path>
    void notify_state_consumers(Path const& p)
    {
      hana::for_each(cells, [&](auto& cell)
      {
        if constexpr(nbdl::StateConsumer<decltype(cell)>::value)
        {
          nbdl::notify_state_change(cell, p);
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
          [&](nbdl::uninitialized)
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
            if constexpr(message::is_upstream<Message>)
            {
              propagate_downstream(m);
            }
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

    // constructor helper
    template <std::size_t ...i, typename ...Args,
      typename = std::enable_if_t<(sizeof...(Args) > 0)>
    >
    context(std::index_sequence<i...>, Args&& ...args)
      : cells(make_cell<i>(std::forward<Args>(args))...)
      , stores()
    { }

    // default constructor helper
    template <std::size_t ...i>
    explicit context(std::index_sequence<i...>)
      : cells(make_cell<i>()...)
      , stores()
    { }

    public:

    using hana_tag = context_tag;

    template <typename Arg1, typename ...Args,
      typename = std::enable_if_t<(
        (sizeof...(Args) + 1 == cell_count)
        && !std::is_same<std::decay_t<Arg1>, context>::value
        && !hana::is_a<hana::ext::std::integer_sequence_tag, Arg1>
      )>
    >
    explicit context(Arg1&& arg1, Args&& ...args)
      : context(
          cell_index_sequence,
          std::forward<Arg1>(arg1),
          std::forward<Args>(args)...
        )
    {
      context_detail::init_producers(cells);
    }

    // default constructor
    context()
      : context(cell_index_sequence)
    {
      context_detail::init_producers(cells);
    }

    // Push functions contain references to self
    // another option is to use shared_from_this,
    // but that responsibility should be on the
    // Producers.
    context(context const&) = delete;

    // Access cell by its position
    // in the tuple of Producers/Consumers
    template <std::size_t i>
    auto& cell()
    {
      return hana::at_c<i>(cells);
    }
  };

  // Store

  template <>
  struct apply_message_impl<context_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store& s, Message&& m)
    {
      s.push_message(std::forward<Message>(m));
      return hana::true_c;
    }
  };

  template <>
  struct get_impl<context_tag>
  {
    template <typename Store, typename Key, typename Fn>
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
    static constexpr void apply(Store& s, Key&& k, Fn&& fn)
    {
      constexpr auto path_type = decltype(hana::typeid_(k)){};
      nbdl::match(s.stores[path_type], k, [&](auto const& value)
      {
        if constexpr(
          decltype(hana::equal(hana::typeid_(value), hana::type_c<nbdl::uninitialized>)){}
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
    static constexpr auto apply(Store& s, Message&& m)
    {
      static_assert(nbdl::UpstreamMessage<Message>::value,
        "nbdl::context::push_upstream_api requires an UpstreamMessage");
      s.ctx.push_message(std::forward<Message>(m));
      return hana::true_c;
    }
  };

  template <>
  struct get_impl<context_detail::store_tag>
  {
    template <typename Store, typename Key>
    static constexpr decltype(auto) apply(Store&& s, Key&& k)
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
      nbdl::match(s.ctx, std::forward<Key>(k), std::forward<Fn>(fn));
    }
  };
}

#endif
