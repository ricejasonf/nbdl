//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_LOCAL_VERSION_HPP
#define NBDL_LOCAL_VERSION_HPP

#include <nbdl/apply_action.hpp>
#include <nbdl/apply_delta.hpp>
#include <nbdl/apply_message.hpp>
#include <nbdl/binder/hash.hpp>
#include <nbdl/concept/State.hpp>
#include <nbdl/delta.hpp>
#include <nbdl/get.hpp>
#include <nbdl/match.hpp>
#include <nbdl/variant.hpp>
#include <nbdl/uid.hpp>

#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/type.hpp>
#include <type_traits>
#include <deque>
#include <unordered_set>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace local_version_detail
  {
    // The definition of "local" in this case is any change
    // that is not confirmed by the root producer
    constexpr auto apply_local_delta = [](auto& store, auto&& uid, auto&& delta) -> bool
    {
      // Add the uid to the local_uids
      store.local_uids.insert(std::forward<decltype(uid)>(uid));

      // All "local" changes go to both 'value' and 'pending_delta'
      nbdl::apply_delta(delta, store.pending_delta);
      return nbdl::apply_delta(std::forward<decltype(delta)>(delta), store.value);
    };

    constexpr auto apply_confirmed_delta = [](auto& store, auto&& uid, auto&& delta) -> bool
    {
      auto uid_el = std::find_if(
        store.local_uids.begin()
      , store.local_uids.end()
      , hana::equal.to(uid)
      );

      // If uid is "pending" then confirmed changes are only applied to
      if (uid_el != store.local_uids.end())
      {
        // Apply to `pending_delta`
        nbdl::apply_delta(delta, store.pending_delta);

        // Remove `uid` from `local_uids`
        store.local_uids.erase(uid_el);

        // If local_uids is now empty then apply pending_delta to value
        if (store.local_uids.empty())
        {
          bool result = nbdl::apply_delta(std::move(store.pending_delta), store.value);
          store.pending_delta = {};
          return result;
        }
        return false;
      }
      else
      {
        // Apply to both `pending_delta` and `value`
        nbdl::apply_delta(delta, store.pending_delta);
        return nbdl::apply_delta(std::forward<decltype(delta)>(delta), store.value);
      }
    };
  }

  template <typename Tag>
  struct local_version_tag { }; // tag

  template <typename T, typename = void>
  struct local_version;

  template <typename T>
  struct local_version<T
                     , std::enable_if_t<
                           nbdl::Store<T>::value
                       && !nbdl::NetworkStore<T>::value
                       >>
  {
    using hana_tag = local_version_tag<hana::tag_of_t<T>>;
    using uid_set = std::unordered_set<nbdl::uid, nbdl::binder::hash_fn, hana::equal_t>;

    T value;
    nbdl::delta<T> pending_delta;
    uid_set local_uids;

    local_version() = default;

    local_version(T x)
      : value(std::move(x))
      , pending_delta()
      , local_uids()
    { }
  };

  template <typename Tag>
  struct apply_message_impl<local_version_tag<Tag>>
  {
    template <typename Store, typename Message>
    static auto apply(Store& store, Message&& m)
    {
      using local_version_detail::apply_confirmed_delta;
      using local_version_detail::apply_local_delta;

      using Payload = typename decltype(
        hana::typeid_(message::get_payload(m))
      )::type;

      static_assert(
          !message::is_update<Message>
      ||  (   nbdl::Delta<Payload>::value
           && hana::is_a<hana::tag_of_t<decltype(store.pending_delta)>, Payload>
          )
      , "Update payload must be a Delta of the same data-type"
      );

      if constexpr(nbdl::Delta<Payload>::value)
      {
        static_assert(
          message::has_uid<Message>
        , "nbdl::local_version requires messages to have a UID"
        );

        // apply delta to store
        if (message::is_confirmed(m))
        {
          apply_confirmed_delta(
            store
          , message::get_uid(std::forward<Message>(m))
          , message::get_payload(std::forward<Message>(m))
          );
        }
        else
        {
          apply_local_delta(
            store
          , message::get_uid(std::forward<Message>(m))
          , message::get_payload(std::forward<Message>(m))
          );
        }
      }
      else
      {
        static_assert(
          !std::is_same<Payload, void>::value
        , "Store message must have a payload for this store."
        );
        return nbdl::apply_action(
          store.value
        , message::get_payload(std::forward<Message>(m))
        );
      }
    }
  };

  template <typename Tag>
  struct match_impl<local_version_tag<Tag>, hana::when<not nbdl::State<Tag>::value>>
  {
    template <typename Store, typename ...Args>
    static void apply(Store& s, Args&& ...args)
    {
      nbdl::match(s.value, std::forward<Args>(args)...);
    }
  };

  template <typename Tag>
  struct get_impl<local_version_tag<Tag>, hana::when<nbdl::State<Tag>::value>>
  {
    template <typename State, typename ...Args>
    static auto apply(State& s, Args&& ...args)
    {
      return nbdl::get(s.value, std::forward<Args>(args)...);
    }
  };
}

#endif
