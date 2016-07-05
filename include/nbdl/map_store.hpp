//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAP_STORE_HPP
#define NBDL_MAP_STORE_HPP

#include <nbdl/fwd/map_store.hpp>

#include <nbdl/make_store.hpp>
#include <nbdl/apply_action.hpp>
#include <nbdl/get.hpp>
#include <nbdl/variant.hpp>

#include <unordered_map>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Message, typename Map>
    auto handle_map_store_action(Message&& m, Map&& map)
    {
      if constexpr(message::is_upstream<Message>)
      {
        if constexpr(message::is_read<Message>)
        {
          // Create `nbdl::unresolved` placeholder to signify that,
          // when the message comes downstream, it is stored.
          if (map.find(message::get_path(m)) == map.end())
          {
            map[message::get_path(m)] = nbdl::unresolved{};
          }
          // Adding the placeholder does not mean that state changed.
          return hana::false_c;
        }
        else
        {
          return hana::false_c;
        }
      }
      else // if downstream
      {
        if constexpr(message::is_create<Message> || message::is_update_raw<Message>)
        {
          map[message::get_path(m)] = *message::get_maybe_payload(m);
          return true;
        }
        else if constexpr(message::is_delete<Message>)
        {
          map.erase(message::get_path(m));
          return true;
        }
        else if constexpr(message::is_read<Message>)
        {
          return map[message::get_path(m)].match(
            [&](nbdl::unresolved)
            {
               map[message::get_path(m)] = *message::get_maybe_payload(m);
               return true;
            },
            [](auto const&) { return false; }
          );
        }
        else
        {
          return hana::false_c;
        }
      }
    };
  } // detail

  template <typename Path>
  struct map_store
  {
    private:

    using Variant = nbdl::variant<typename Path::Entity, nbdl::not_found>;
    using HashFn = typename Path::hash_fn;
    using PredFn = typename Path::pred_fn;
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
    template <typename PathType>
    static constexpr auto apply(PathType)
      -> nbdl::map_store<typename PathType::type>
    { return {}; }
  };

  template <>
  struct apply_action_impl<map_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&& s, Message&& m)
    {
      using Path = typename std::decay_t<Store>::path;
      static_assert(
        decltype(hana::type_c<typename Path::canonical_path> == message::get_path_type(m))::value
      );
      return handle_map_store_action(std::forward<Message>(m), std::forward<Store>(s).map);
    }
  };

  template <>
  struct match_impl<map_store_tag>
  {
    template <typename Store, typename Path, typename ...Fn>
    static constexpr auto apply(Store&& s, Path&& p, Fn&&... fn)
      ->  nbdl::detail::common_type_t<
            decltype((*(s.map.find(std::forward<Path>(p)))).match(std::forward<Fn>(fn)...)),
            decltype(hana::overload_linearly(std::forward<Fn>(fn)...)(nbdl::uninitialized{}))
          >
    {
      auto node = s.map.find(std::forward<Path>(p));
      if (node != s.map.end())
      {
        return (*node).match(std::forward<Fn>(fn)...);
      }
      else
      {
        return hana::overload_linearly(std::forward<Fn>(fn)...)(nbdl::uninitialized{});
      }
    }
  };
} // nbdl

#endif
