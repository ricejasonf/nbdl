//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include<nbdl/concepts/UpstreamMessage.hpp>
#include<nbdl/concepts/DownstreamMessage.hpp>
#include<nbdl/dispatch.hpp>
#include<nbdl/receive_message.hpp>

#include<boost/hana.hpp>
#include<boost/hana/ext/std/integer_sequence.hpp>
#include<type_traits>
#include<utility>

namespace nbdl {

namespace hana = boost::hana;

template<
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

    template<typename Message>
    void operator()(Message&& m)
    {
      static_assert(UpstreamMessage<Message>::value,
        "nbdl::context::push_upstream requires an UpstreamMessage");
       ctx.pushUpstream(std::forward<Message>(m));
    }
  };

  struct push_downstream_fn
  {
    context& ctx;

    push_downstream_fn(context& c) : ctx(c) { }

    template<typename Message>
    void operator()(Message&& m)
    {
      static_assert(DownstreamMessage<Message>::value,
        "nbdl::context::push_downstream requires a DownMessage");
       ctx.pushDownstream(std::forward<Message>(m));
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

  template<typename Cell, typename PushFn, bool>
  struct make_cell_helper;

  template<std::size_t i, typename... Args>
  auto make_cell(Args&& ...args)
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

  template<typename Message>
  void propagate_message(Message&& m, message::channel::upstream)
  {
    using Index = decltype(hana::at_key(ProviderLookup{}, hana::decltype_(message::get_path(m))));
    constexpr Index index{};
    nbdl::receive_message(cells[index], std::forward<Message>(m));
  }

  template<typename Message>
  void propagate_message(Message&& m, message::channel::downstream)
  {
    using Index = decltype(hana::at_key(ConsumerLookup{}, hana::decltype_(message::get_path(m))));
    constexpr Index index{};
    nbdl::receive_message(cells[index], std::forward<Message>(m));
  }

  // called inside PushUpstreamMessageFn and PushDownstreamMessageFn
  template<typename Message>
  void push_message(Message&& m)
  {
    constexpr auto path_type = hana::decltype_(message::get_path(m));
    constexpr auto channel = decltype(message::get_channel(m)){};
    propagate_message(
      nbdl::dispatch(
        stores[path_type],
        std::forward<Message>(m)
      ),
      channel
    );
  }

  // constructor helper
  template<std::size_t ...i, typename ...Args,
    typename = std::enable_if_t<(sizeof...(Args) > 0)>
  >
  context(std::index_sequence<i...>, Args&& ...args)
    : cells(make_cell<i>(args)...)
    , stores()
  { }

  // default constructor helper
  template<std::size_t ...i>
  explicit context(std::index_sequence<i...>)
    : cells(make_cell<i>()...)
    , stores()
  { }

	public:

  template<typename Arg1, typename ...Args,
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
