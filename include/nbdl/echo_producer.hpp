//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ECHO_PROVIDER_HPP
#define NBDL_ECHO_PROVIDER_HPP

#include<nbdl/fwd/echo_producer.hpp>

#include<nbdl/entity.hpp>
#include<nbdl/make_producer.hpp>
#include<nbdl/message.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace echo_producer_detail
  {
    template <typename Message>
    auto make_unique_key(Message const& m)
    {
      using Path = typename decltype(message::get_path_type(m))::type;
      using Key = std::decay_t<decltype(hana::back(std::declval<Path>()))>;

      if constexpr(std::is_empty<Key>::value)
      {
        return Key{};
      }
      else if constexpr(std::is_integral<Key>::value)
      {
        static Key key = 0;
        return ++key;
      }
      else
      {
        // Only supports keys that wrap an integral named "value"
        using Integral = typename NBDL_MEMBER(&Key::value)::member_type;
        static Integral key = 0;
        return Key{++key};
      }
    }
  }

  template <typename PushApi>
  struct echo_producer_impl
  {
    using hana_tag = echo_producer;

    PushApi push_api;
  };

  template <>
  struct make_producer_impl<echo_producer>
  {
    template <typename PushApi>
    static constexpr auto apply(PushApi&& p)
    {
      return echo_producer_impl<PushApi>{std::forward<PushApi>(p)};
    }
  };

  template <>
  struct send_upstream_message_impl<echo_producer>
  {
    template <typename Producer, typename Message>
    static constexpr void apply(Producer const& p, Message const& m)
    {
      if constexpr(message::is_create<Message>)
      {
        p.push_api.push(p.push_api.message_api().to_downstream_from_root(
          m,
          echo_producer_detail::make_unique_key(m)
        ));
      }
      else
      {
        p.push_api.push(p.push_api.to_downstream_from_root(m));
      }
    }
  };
} // nbdl

#endif