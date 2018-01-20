//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAP_STORE_HPP
#define NBDL_MAP_STORE_HPP

#include <nbdl/fwd/map_store.hpp>

#include <nbdl/apply_message.hpp>
#include <nbdl/binder/hash.hpp>
#include <nbdl/ext/std/unordered_map.hpp>
#include <nbdl/get.hpp>
#include <nbdl/make_store.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/equal.hpp>
#include <unordered_map>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Message>
    struct handle_map_store_action
    {
      template <typename Map, typename Message_>
      static hana::false_ apply(Map&&, Message_&&)
      { return {}; }
    };

    template <typename ...T>
    struct handle_map_store_action<message::upstream_read<T...>>
    {
      template <typename Map, typename Message>
      static hana::false_ apply(Map&& map, Message&& m)
      {
        static_assert(
          message::is_upstream<Message> &&
          message::is_read<Message>
        );
        // Create `nbdl::unresolved` placeholder to signify that,
        // when the message comes downstream, it is stored.
        if (map.find(message::get_path(m)) == map.end())
        {
          map[message::get_path(std::forward<Message>(m))] = nbdl::unresolved{};
        }
        // Adding the placeholder does not mean that state changed.
        return {};
      }
    };

    template <typename ...T>
    struct handle_map_store_action<message::downstream_create<T...>>
    {
      template <typename Map, typename Message>
      static bool apply(Map&& map, Message&& m)
      {
        auto path = message::get_path(m);
        map[path] = message::get_payload(std::forward<Message>(m));

        return true;
      }
    };

    template <typename ...T>
    struct handle_map_store_action<message::downstream_update<T...>>
    {
      template <typename Map, typename Message>
      static bool apply(Map&& map, Message&& m)
      {
        auto path = message::get_path(m);
        map[std::move(path)] = message::get_payload(std::forward<Message>(m));
        return true;
      }
    };

    template <typename ...T>
    struct handle_map_store_action<message::downstream_read<T...>>
    {
      template <typename Map, typename Message>
      static bool apply(Map&& map, Message&& m)
      {
        bool result = false;
        nbdl::match_when<nbdl::unresolved>(map[message::get_path(m)], [&](nbdl::unresolved)
        {
          auto path = message::get_path(m);
          map[std::move(path)] = message::get_payload(std::forward<Message>(m));
          result = true;
        });

        return result;
      }
    };

    template <typename ...T>
    struct handle_map_store_action<message::downstream_delete<T...>>
    {
      template <typename Map, typename Message>
      static bool apply(Map&& map, Message&& m)
      {
        map.erase(message::get_path(m));
        return true;
      }
    };
  }

  template <typename Path, typename Entity>
  struct map_store
  {
    private:

    using Variant = nbdl::variant<Entity, nbdl::not_found>;
    using HashFn = nbdl::binder::hash_fn;
    using PredFn = hana::equal_t;
    using Container = std::unordered_map<Path, Variant, HashFn, PredFn>;

    public:

    using hana_tag = nbdl::map_store_tag;
    using path = Path;

    Container map;

    map_store()
      : map()
    { }
  };


  template <>
  struct make_store_impl<map_store_tag>
  {
    template <typename PathType, typename EntityType>
    static constexpr auto apply(PathType, EntityType)
      -> nbdl::map_store<typename PathType::type, typename EntityType::type>
    { return {}; }
  };

  template <>
  struct apply_message_impl<map_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m)
    {
      return detail::handle_map_store_action<std::decay_t<Message>>
        ::apply(std::forward<Store>(s).map ,std::forward<Message>(m));
    }
  };

  template <>
  struct match_impl<map_store_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      nbdl::match(s.map, std::forward<Key>(k), std::forward<Fn>(fn));
    }
  };
}

#endif
