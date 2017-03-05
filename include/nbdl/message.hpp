//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MESSAGE_HPP
#define NBDL_MESSAGE_HPP

#include <nbdl/fwd/message.hpp>

#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/detail/normalize_path_type.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/optional.hpp>

namespace nbdl { namespace message
{

  namespace hana = boost::hana;

  namespace detail
  {
    // Hack UpstreamMessages.
    // Since they never have is_from_root,
    // it is just removed from the sequence
    // This is just a really hacky way to
    // remap a indices.
    template <typename M, int i, typename = void>
    struct fix_index_t
    {
      static constexpr int value = i;
    };

    template <typename M>
    struct fix_index_t<M, 5, std::enable_if_t<nbdl::UpstreamMessage<M>::value>>
    {
      static constexpr int value = 4;
    };

    template <typename M>
    struct fix_index_t<M, 6, std::enable_if_t<nbdl::UpstreamMessage<M>::value>>
    {
      static constexpr int value = 5;
    };

    template <typename Message, int i>
    constexpr auto fix_index = hana::size_c<fix_index_t<Message, i>::value>;

    template <int i>
    struct get_fn
    {
      template <typename M>
      constexpr decltype(auto) operator()(M&& m) const
      {
        return hana::at(std::forward<M>(m), hana::int_c<i>);
      }
    };

    template <int i>
    struct get_maybe_fn
    {
      template <typename M>
      constexpr decltype(auto) operator()(
        M const&,
        std::enable_if_t<
          (nbdl::UpstreamMessage<M>::value &&  i == 4)
          || decltype(hana::length(std::declval<M>()) <= detail::fix_index<M, i>)::value
        , int> = 0
      ) const
      {
        return hana::nothing;
      }

      template <typename M>
      constexpr decltype(auto) operator()(M&& m,
        std::enable_if_t<!(
          (nbdl::UpstreamMessage<M>::value &&  i == 4)
          || decltype(hana::length(std::declval<M>()) <= detail::fix_index<M, i>)::value
        ), int> = 0
      ) const
      {
        return hana::at(std::forward<M>(m), detail::fix_index<M, i>);
      }
    };

    // get contents of maybe or fail at compile-time
    template <int i>
    struct get_just_fn
    {
      template <typename Message>
      constexpr decltype(auto) operator()(Message&& m) const
      {
        return *get_maybe_fn<i>{}(std::forward<Message>(m));
      }
    };
  } // detail

  // The offsets of these properties should match the
  // formation of the message in `builder::EntityMessage`.
  constexpr detail::get_fn      < 0 > get_channel{};
  constexpr detail::get_fn      < 1 > get_action{};
  constexpr detail::get_fn      < 2 > get_path{};
  constexpr detail::get_maybe_fn< 3 > get_maybe_uid{};
  constexpr detail::get_maybe_fn< 4 > get_maybe_is_from_root{};
  constexpr detail::get_maybe_fn< 5 > get_maybe_payload{};
  constexpr detail::get_maybe_fn< 6 > get_maybe_private_payload{};

  // convenience functions for when it should be just
  constexpr detail::get_just_fn< 3 > get_uid{};
  constexpr detail::get_just_fn< 4 > get_is_from_root{};
  constexpr detail::get_just_fn< 5 > get_payload{};
  constexpr detail::get_just_fn< 6 > get_private_payload{};

  // The path type is used as a key in a few places.
  // If the last element is a hana type then extract
  // that type. (It is used as a placeholder for create
  // messages)
  struct get_path_type_fn
  {
    template <typename Message>
    constexpr auto operator()(Message const& m) const
    {
      return nbdl::detail::normalize_path_type(hana::typeid_(message::get_path(m)));
    }
  };

  constexpr get_path_type_fn get_path_type{};

  struct get_entity_type_fn
  {
    template <typename Message>
    constexpr auto operator()(Message const& m) const
    {
      return hana::type_c<typename std::decay_t<decltype(message::get_path(m))>::Entity>;
    }
  };

  constexpr get_entity_type_fn get_entity_type{};

  template <typename Message>
  constexpr bool is_upstream = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<0>))>,
      message::channel::upstream
    >::value;

  template <typename Message>
  constexpr bool is_downstream = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<0>))>,
      message::channel::downstream
    >::value;

  template <typename Message>
  constexpr bool is_create = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<1>))>,
      message::action::create
    >::value;

  template <typename Message>
  constexpr bool is_read = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<1>))>,
      message::action::read
    >::value;

  template <typename Message>
  constexpr bool is_update = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<1>))>,
      message::action::update
    >::value;

  template <typename Message>
  constexpr bool is_update_raw = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<1>))>,
      message::action::update_raw
    >::value;


  template <typename Message>
  constexpr bool is_delete = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<Message>(), hana::int_c<1>))>,
      message::action::delete_
    >::value;
}} // nbdl::message
#endif
