//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include <nbdl/concept/Consumer.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/concept/Provider.hpp>
#include <nbdl/concept/StateConsumer.hpp>
#include <nbdl/detail/concept_pred.hpp>
#include <nbdl/apply_action.hpp>
#include <nbdl/match.hpp>
#include <nbdl/message.hpp>
#include <nbdl/send_downstream_message.hpp>
#include <nbdl/send_upstream_message.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/hana/ext/std/integer_sequence.hpp>
#include <type_traits>
#include <utility>

namespace nbdl {

  namespace hana  = boost::hana;
  namespace hanax = boost::hana::experimental;

  template <
    typename ProviderLookup,
    typename ConsumerLookup,
    typename CellTagTypes,
    typename StoreMap,
    typename MessageApi,
    typename ArgTypes >
  class context
  {
    // calling the push functions after
    // the context is destroyed will result
    // in undefined behaviour
    class push_upstream_api
      : public MessageApi
    {
      context& ctx;

      public:

      push_upstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m)
      {
        static_assert(nbdl::UpstreamMessage<Message>::value,
          "nbdl::context::push_upstream_api requires an UpstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }
    };

    class push_downstream_api
      : public MessageApi
    {
      context& ctx;

      public:

      push_downstream_api(context& c) : ctx(c) { }

      template <typename Message>
      void push(Message&& m)
      {
        static_assert(nbdl::DownstreamMessage<Message>::value,
          "nbdl::context::push_downstream_api requires a DownstreamMessage");
         ctx.push_message(std::forward<Message>(m));
      }
    };

    static constexpr std::size_t cell_count = decltype(hana::size(CellTagTypes{}))::value;
    static constexpr auto cell_index_sequence = std::make_index_sequence<cell_count>{};

    static constexpr std::size_t provider_count = decltype(hana::at(
      hana::take_back(
        hana::unpack(ProviderLookup{},
          mpdef::make_list ^hana::on^ hana::second
        ),
        hana::size_c<1>
      ),
      hana::size_c<0>
    ) + hana::size_c<1>)::value;

    static constexpr std::size_t consumer_count = decltype(hana::count_if(CellTagTypes{},
      hana::compose(detail::concept_pred<Consumer>, hana::traits::declval)))::value;

    template <typename CellTag>
    static constexpr auto cell_factory_helper(std::enable_if_t<
      nbdl::Provider<CellTag>::value
    , int> = 0)
    { return nbdl::make_provider<CellTag>; }

#if 0 // TODO: Support StateConsumer
    template <typename CellTag>
    static constexpr auto cell_factory_helper(std::enable_if_t<
      nbdl::StateConsumer<CellTag>::value
    , int> = 0)
    { return nbdl::make_state_consumer<CellTag>; }
#endif

    template <typename CellTag>
    static constexpr auto cell_factory_helper(std::enable_if_t<
          nbdl::Consumer<CellTag>::value
      && !nbdl::StateConsumer<CellTag>::value
    , int> = 0)
    { return nbdl::make_consumer<CellTag>; }

    template <std::size_t i>
    static constexpr auto cell_factory()
    {
      using CellTag = typename decltype(hana::at_c<i>(CellTagTypes{}))::type;
      return cell_factory_helper<CellTag>();
    }

    template <typename CellTag>
    static constexpr auto get_push_api_type_helper(std::enable_if_t<
      nbdl::Provider<CellTag>::value
    , int> = 0)
    { return hana::type_c<push_downstream_api>; }

#if 0 // TODO: Support StateConsumer
    // Remember that a StateConsumer can also be a Consumer
    template <typename CellTag>
    static constexpr auto get_push_api_type_helper(std::enable_if_t<
      nbdl::StateConsumer<CellTag>::value
    , int> = 0)
    { return hana::type_c<TBD>; }
#endif

    template <typename CellTag>
    static constexpr auto get_push_api_type_helper(std::enable_if_t<
          nbdl::Consumer<CellTag>::value
      && !nbdl::StateConsumer<CellTag>::value
    , int> = 0)
    { return hana::type_c<push_upstream_api>; }

    template <std::size_t i>
    static constexpr auto get_push_api_type()
    {
      using CellTag = typename decltype(hana::at_c<i>(CellTagTypes{}))::type;
      return get_push_api_type_helper<CellTag>();
    }

    template <std::size_t i>
    using PushApi = typename decltype(get_push_api_type<i>())::type;

    template <std::size_t i>
    static constexpr auto make_cell_type()
    {
      return hana::type_c<decltype(cell_factory<i>()(std::declval<PushApi<i>>()))>;
    }
    
    template <std::size_t i, typename ArgType>
    static constexpr auto make_cell_type(ArgType arg_type)
    {
      return hana::type_c<decltype(cell_factory<i>()(std::declval<PushApi<i>>(),
         hana::traits::declval(arg_type)))>;
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
        hana::traits::declval(make_cell_type<i1>(hana::type_c<Arg1>)),
        hana::traits::declval(make_cell_type<i>(hana::type_c<Args>))...
      ))>;
    }

    using Cells = typename decltype(make_storage_helper(cell_index_sequence, ArgTypes{}))::type;

    Cells cells;
    StoreMap stores;

    template <std::size_t i, typename... Args>
    decltype(auto) make_cell(Args&& ...args)
    {
      return cell_factory<i>()(PushApi<i>(*this), std::forward<Args>(args)...);
    }

    template <typename Message>
    void propagate_message(Message&& m)
    {
        if constexpr(message::is_upstream<Message>)
        {
          using Path = std::decay_t<decltype(message::get_path(m))>;
          using Index = decltype(hana::at_key(ProviderLookup{}, hana::type_c<Path>));
          nbdl::send_upstream_message(cells[Index{}], std::forward<Message>(m));
        }
        else
        {
          propagate_downstream(std::forward<Message>(m));
        }
    }

    template <typename Message>
    void propagate_downstream(Message const& m)
    {
      if constexpr(consumer_count > 0)
      {
        if constexpr(message::is_upstream<Message>)
        {
          propagate_downstream(MessageApi{}.to_downstream(m));
        }
        else
        {
          // notify all consumers
          hana::for_each(hana::make_range(hana::size_c<provider_count>, hana::length(cells)),
            [&](auto i)
            {
              nbdl::send_downstream_message(cells[i], m);
            });
        }
      }
    }

    // All actions MUST be applied to the
    // store BEFORE the message is propagated!

    // called inside PushUpstreamMessageFn and PushDownstreamMessageFn
    template <typename Message>
    void push_message(Message&& m)
    {
      constexpr auto path_type = decltype(message::get_path_type(m)){};
      auto& store = stores[path_type];
      if constexpr(message::is_upstream<Message> && message::is_read<Message>)
      {
        nbdl::apply_action(store, m);
        // send response message
        nbdl::match(store, message::get_path(m),
          [&](nbdl::uninitialized)
          {
            // The Store should create an 'unresolved'
            // placeholder to prevent duplicate upstream
            // read requests and so any StateConsumer
            // gets a value
            propagate_message(m);
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
            // and don't bother the Provider with it.
            propagate_message(
              MessageApi{}.to_downstream(m, std::forward<decltype(value)>(value))
            );
          }
        );
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
        if (nbdl::apply_action(store, m))
        {
          // the state changed
          if constexpr(message::is_upstream<Message>)
          {
            propagate_downstream(m);
          }
          //notify_state_consumers(message::get_path(m));
        }
        propagate_message(m);
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
            //notify_state_consumers(path);
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
    // Providers.
    context(context const&) = delete;

    // Access cell by its position
    // in the tuple of Providers/Consumers
    template <std::size_t i>
    auto& cell()
    {
      return hana::at_c<i>(cells);
    }
  };
}//nbdl

#endif
