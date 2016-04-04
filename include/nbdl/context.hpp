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
#include <nbdl/concept/StateConsumer.hpp>
#include <nbdl/detail/concept_pred.hpp>
#include <nbdl/apply_action.hpp>
#include <nbdl/message.hpp>
#include <nbdl/send_downstream_message.hpp>
#include <nbdl/send_upstream_message.hpp>

#include <boost/hana.hpp>
#include <boost/hana/ext/std/integer_sequence.hpp>
#include <type_traits>
#include <utility>

namespace nbdl {

namespace hana = boost::hana;

template <
	typename ProviderLookup,
  typename ConsumerLookup,
  typename MakeContextCells,
	typename StoreMap>
class context
{
  // calling the push functions after
  // the context is destroyed will result
  // in undefined behaviour
  struct push_upstream_fn
  {
    context& ctx;

    push_upstream_fn(context& c) : ctx(c) { }

    template <typename Message>
    void operator()(Message&& m)
    {
      static_assert(UpstreamMessage<Message>::value,
        "nbdl::context::push_upstream_fn requires an UpstreamMessage");
       ctx.push_message(std::forward<Message>(m));
    }
  };

  struct push_downstream_fn
  {
    context& ctx;

    push_downstream_fn(context& c) : ctx(c) { }

    template <typename Message>
    void operator()(Message&& m)
    {
      static_assert(DownstreamMessage<Message>::value,
        "nbdl::context::push_downstream_fn requires a DownMessage");
       ctx.push_message(std::forward<Message>(m));
    }
  };

  using Cells = typename decltype(MakeContextCells{}(
    hana::type_c<push_upstream_fn>,
    hana::type_c<push_downstream_fn>
  ))::type;

  Cells cells;
	StoreMap stores;

  static constexpr std::size_t cell_count = decltype(hana::size(std::declval<Cells>()))::value;

  static constexpr std::size_t provider_count = decltype(hana::at(
    hana::take_back(
      hana::unpack(ProviderLookup{},
        mpdef::make_list ^hana::on^ hana::second
      ),
      hana::size_c<1>
    ),
    hana::size_c<0>
  ) + hana::size_c<1>)::value;

  static constexpr std::size_t consumer_count = decltype(hana::count_if(cells,
    detail::concept_pred<Consumer>))::value;

  template <typename Cell, typename PushFn, bool>
  struct make_cell_helper;

  template <std::size_t i, typename... Args>
  decltype(auto) make_cell(Args&& ...args)
  {
    using Cell = std::decay_t<decltype(hana::at_c<i>(std::declval<Cells>()))>;
    using PushFn = typename decltype(
      hana::if_(hana::bool_c<(i < provider_count)>, // is provider?
        hana::type_c<push_downstream_fn>,
        hana::type_c<push_upstream_fn>
      )
    )::type;

    return Cell(PushFn(*this), std::forward<Args>(args)...);
  }

  template <typename Message>
  void propagate_message(Message const& m, message::channel::upstream)
  {
    using Index = decltype(hana::at_key(ProviderLookup{}, hana::decltype_(message::get_path(m))));
    constexpr Index index{};
    nbdl::send_upstream_message(cells[index], m);
  }

  template <typename Message>
  void propagate_message(Message&& m, message::channel::downstream)
  {
    propagate_downstream(std::forward<Message>(m), message::channel::downstream{});
  }

  template <typename Message>
  void propagate_downstream(Message const&, message::channel::upstream,
    hana::when<(consumer_count == 0)> = {})
  { }

  template <typename Message>
  void propagate_downstream(Message const& m, message::channel::upstream,
    hana::when<(consumer_count > 0)> = {})
  {
    // TODO create the appropriate downstream message
    // propagate_downstream(new_message, message::channel::downstream{});
  }

  template <typename Message>
  void propagate_downstream(Message const& m, message::channel::downstream)
  {
    // notify all consumers
    hana::while_(hana::less.than(hana::length(cells)), provider_count,
      [&](auto i) {
        nbdl::send_downstream_message(cells[i], m);
      });
  }

  // All actions MUST be applied to the
  // store BEFORE the message is propagated!

  template <typename Message>
  void dispatch(Message const& m, message::channel::upstream, message::action::read)
  {
    constexpr auto path_type = decltype(message::get_path_type(m)){};
    auto& store = stores[path_type];
    if (nbdl::has(store, message::get_path(m)))
    {
      nbdl::apply_action(store, m);
      // TODO send response message
    }
    else
    {
      nbdl::apply_action(store, m);
      propagate_message(m, message::get_channel(m));
    }
  }

#if 0
  template <typename Message>
  void dispatch(Message const& m, message::channel::downstream, message::action::delta)
  {
    // TODO Github issue #15: Handle Downstream Detlas
    constexpr auto path_type = decltype(message::get_path_type(m)){};
    auto& store = stores[path_type];
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

  template <typename Message>
  void dispatch(Message const& m, ...)
  {
    constexpr auto path_type = decltype(message::get_path_type(m)){};
    auto& store = stores[path_type];
    if (nbdl::apply_action(store, m))
    {
      // the state changed
      propagate_downstream(m, message::get_channel(m));
      notify_state_consumers(message::get_path(m));
    }
    propagate_message(m, message::get_channel(m));
  }

  // called inside PushUpstreamMessageFn and PushDownstreamMessageFn
  template <typename Message>
  void push_message(Message const& m)
  {
    // TODO make dispatcher struct for this stuff
    constexpr auto path_type = decltype(message::get_path_type(m)){};
    constexpr auto channel = decltype(message::get_channel(m)){};
    if (handle_read(m))
    {
      // the store already had the value
      // and a downstream message was sent
      return;
    }
    else if (nbdl::apply_action(stores[path_type], m))
    {
      // the state changed
      propagate_downstream(m, message::get_channel(m));
      notify_state_consumers(message::get_path(m));
    }
    // propagate the original message except for upstream
    // reads when the value is already present in the store
    // (meaning no state was changed because it already has
    // the value)
  }

  // constructor helper
  template <std::size_t ...i, typename ...Args,
    typename = std::enable_if_t<(sizeof...(Args) > 0)>
  >
  context(std::index_sequence<i...>, Args&& ...args)
    : cells(make_cell<i>(args)...)
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
        std::make_index_sequence<sizeof...(Args) + 1>{},
        std::forward<Arg1>(arg1),
        std::forward<Args>(args)...
      )
  { }

  // default constructor
  context()
    : context(std::make_index_sequence<cell_count>{})
  { }

  // Push functions contain references to self
  // another option is to use shared_from_this,
  // but that responsibility should be on the
  // Providers.
  context(context const&) = delete;
};

}//nbdl

#endif
