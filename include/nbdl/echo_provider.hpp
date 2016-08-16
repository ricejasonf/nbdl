//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ECHO_PROVIDER_HPP
#define NBDL_ECHO_PROVIDER_HPP

#include<nbdl/fwd/echo_provider.hpp>

#include<nbdl/make_provider.hpp>
#include<nbdl/message.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace echo_provider_detail
  {
    template <typename Message>
    auto make_unique_key(Message const& m)
    {
      using Key = typename std::decay_t<decltype(message::get_path(m))>::canonical_key;

      if constexpr(std::is_integral<Key>::value)
      {
        static Key key = 0;
        return ++key;
      }
      else
      {
        static_assert(std::is_empty<Key>::value);
        return Key{};
      }
    }
  }

  template <typename PushApi>
  struct echo_provider_impl
  {
    using hana_tag = echo_provider;

    PushApi push_api;
  };

  template <>
  struct make_provider_impl<echo_provider>
  {
    template <typename PushApi>
    static constexpr auto apply(PushApi&& p)
    {
      return echo_provider_impl<PushApi>{std::forward<PushApi>(p)};
    }
  };

  template <>
  struct send_upstream_message_impl<echo_provider>
  {
    template <typename Provider, typename Message>
    static constexpr void apply(Provider const& p, Message const& m)
    {
      if constexpr(message::is_create<Message>)
      {
        p.push_api.push(p.push_api.message_api().to_downstream_from_root(
          m,
          echo_provider_detail::make_unique_key(m)
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
