//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include <nbdl/def/builder/context.hpp>
#include <nbdl/concept/Consumer.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/concept/Producer.hpp>
#include <nbdl/concept/StateConsumer.hpp>
#include <nbdl/detail/concept_pred.hpp>
#include <nbdl/detail/normalize_path_type.hpp>
#include <nbdl/apply_action.hpp>
#include <nbdl/match.hpp>
#include <nbdl/message.hpp>
#include <nbdl/message_api.hpp>
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

  namespace detail
  {
    struct context_store_tag { };
  }

  template <typename Tag, typename ArgTypes>
  class context
  {
    using ContextMeta     = nbdl_def::builder::make_context_meta_t<Tag, ArgTypes>;
    using ProducerLookup  = typename ContextMeta::producer_lookup;
    using CellTagTypes    = typename ContextMeta::cell_tag_types;
    using StoreMap        = typename ContextMeta::store_map;
    using MessageApi      = nbdl::message_api<context>;

    public:

    using message_api_meta = typename ContextMeta::message_api_meta;

    private:

    // calling the push functions after
    // the context is destroyed will result
    // in undefined behaviour
    class push_upstream_api
    {
      context& ctx;

      public:

      push_upstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m) const
      {
        static_assert(nbdl::UpstreamMessage<Message>::value,
          "nbdl::context::push_upstream_api requires an UpstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }

      constexpr MessageApi message_api() const { return {}; }
    };

    class push_downstream_api
    {
      context& ctx;

      public:

      push_downstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m) const
      {
        static_assert(nbdl::DownstreamMessage<Message>::value,
          "nbdl::context::push_downstream_api requires a DownstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }

      constexpr MessageApi message_api() const { return {}; }
    };

    class push_upstream_api_state_consumer
    {
      context& ctx;

      public:

      push_upstream_api_state_consumer(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m) const
      {
        static_assert(nbdl::UpstreamMessage<Message>::value,
          "nbdl::context::push_upstream_api_state_consumer requires an UpstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }

      template <typename Path, typename ...Fns>
      void match(Path&& path, Fns&& ...fns) const
      {
        ctx.match(std::forward<Path>(path), std::forward<Fns&&>(fns)...);
      }

      constexpr MessageApi message_api() const { return {}; }

#if 0
      template <typename PathSpec, typename ...Payloads>
      void create(PathSpec, Payloads&& ...ps)
      {
        MessageApi{}.make_upstream_create_message(
          nbdl::detail::resolve_path<PathSpec>(*this)
        , std::forward<Payloads>(ps)...
        );
      }

      template <typename PathSpec, typename Payload>
      void update(PathSpec, Payload&& p)
      {
        MessageApi{}.make_upstream_update_raw_message(
          nbdl::detail::resolve_path<PathSpec>(*this)
        , std::forward<Payload>(p)
        );
      }

      template <typename PathSpec>
      void delete_(PathSpec)
      {
        MessageApi{}.make_upstream_delete_message(
          nbdl::detail::resolve_path<PathSpec>(*this)
        , std::forward<Payload>(p)
        );
      }
#endif

      using hana_tag = detail::context_store_tag;
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
          ProducerLookup{}, 
          message::get_path_type(m)
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

    // called inside push_upstream_api_state_consumer
    template <typename Path, typename ...Fns>
    decltype(auto) match(Path&& p, Fns&& ...fns)
    {
      constexpr auto path_type = decltype(hana::traits::decay(hana::decltype_(p))){};
      return nbdl::match(stores[path_type], p, hana::overload_linearly(
        [&](nbdl::uninitialized)
        {
          if constexpr(decltype(MessageApi{}.has_upstream_read(p))::value)
          {
            // Trigger upstream read request.
            // The store's value should be set to `nbdl::unresolved`
            push_message(MessageApi{}.make_upstream_read_message(p));
            return hana::overload_linearly(std::forward<Fns>(fns)...)(nbdl::unresolved{});
          }
        },
        [&](auto const& value)
        {
          // Users should not be allowed to modify values directly.
          return hana::overload_linearly(std::forward<Fns>(fns)...)(value);
        }
      ));
    }

    // All actions MUST be applied to the
    // store BEFORE the message is propagated!

    // called inside PushUpstreamMessageFn and PushDownstreamMessageFn
    template <typename Message>
    auto push_message(Message const& m)
    {
      constexpr auto path_type = decltype(message::get_path_type(m)){};
      auto& store = stores[path_type];
      if constexpr(message::is_upstream<Message> && message::is_read<Message>)
      {
        nbdl::apply_action(store, m);
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
              MessageApi{}.to_downstream(m, std::forward<decltype(value)>(value))
            );
          }
        ));
      }
#if 0
      else if constexpr(message::is_downstream<Message> && message::is_update<Message>)
      {
      // TODO Github issue #15: Handle Downstream Deltas by squashing them
      nbdl::apply_action_downstream_delta(store, m).match(
        [](auto&& d)
          -> std::enable_if_t<nbdl::Delta<decltype(d)>::value>
        {
          // TODO create new downstream message with `d` as the payload
        },
        nbdl::noop
      );
      }
#endif
      else
      {
        if constexpr(!decltype(hana::equal(
          hana::type_c<decltype(nbdl::apply_action(store, m))>,
          hana::type_c<hana::false_>
        ))::value)
        {
          if (nbdl::apply_action(store, m))
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

      // apply the action to ALL OTHER stores
      hana::for_each(stores, [&](auto& pair)
      {
        if constexpr(!decltype(hana::equal(hana::first(pair), path_type))::value)
        {
          nbdl::apply_foreign_action(hana::second(pair), m, [&](auto const& path)
          {
            static_assert(
              decltype(
                hana::equal(
                  hana::traits::decay(hana::decltype_(path)),
                  hana::first(pair)
                )
              )::value
              , "Store can only mark its own path as modified.");
            notify_state_consumers(path);
          });
        }
      });
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
    { }

    // default constructor
    context()
      : context(cell_index_sequence)
    { }

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

  // As a Store for StateConsumers

  template <>
  struct apply_action_impl<detail::context_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store& s, Message&& m)
    {
      s.push(std::forward<Message>(m));
      return hana::true_c;
    }
  };

  template <>
  struct match_impl<detail::context_store_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr auto apply(Store&& s, Key&& k, Fn&& fn)
    {
      s.match(std::forward<Key>(k), std::forward<Fn>(fn));
    }
  };
}//nbdl

#endif
