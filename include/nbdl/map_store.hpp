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
#include <nbdl/detail/message_dispatch.hpp>
#include <nbdl/get.hpp>
#include <nbdl/variant.hpp>

#include <unordered_map>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    // TODO move the logic for each action here
    struct map_store_action_handler
    {
      template <typename Message, typename Map>
      bool handle_upstream_read(Message const& m, Map&& map)
      {
        if (map.find(message::get_path(m)) != map.end())
        {
          return false;
        }
        else
        {
          map[message::get_path(m)] = nbdl::unresolved{};
          return true;
        }
      }

      template <typename Message, typename Map>
      bool handle_downstream_read(Message&& m, Map&& map)
      {
        return map[message::get_path(m)].match(
          [&](nbdl::unresolved)
          {
             map[message::get_path(m)] = *message::get_maybe_payload(m);
             return true;
          },
          [](auto const&)
          {
            return false;
          }
        );
      }

      template <typename Message, typename Map>
      bool handle_downstream_read_not_found(Message&& m, Map&& map)
      {
        return map[message::get_path(m)].match(
          [&](nbdl::unresolved)
          {
             map[message::get_path(m)] = nbdl::not_found{};
             return true;
          },
          [](auto const&)
          {
            return false;
          }
        );
      }

      template <typename Message, typename Map>
      bool handle_upstream_create(Message&&, Map&&)
      {
        return false;
      }

      template <typename Message, typename Map>
      bool handle_downstream_create(Message&& m, Map&& map)
      {
        map[message::get_path(m)] = *message::get_maybe_payload(m);
        return true;
      }

      // TODO: handle diffs

      template <typename Message, typename Map>
      bool handle_upstream_update_raw(Message&&, Map&&)
      {
        return false;
      }

      template <typename Message, typename Map>
      bool handle_downstream_update_raw(Message&& m, Map&& map)
      {
        map[message::get_path(m)] = *message::get_maybe_payload(m);
        return true;
      }

      template <typename Message, typename Map>
      bool handle_upstream_delete(Message&&, Map&&)
      {
        return false;
      }

      template <typename Message, typename Map>
      bool handle_downstream_delete(Message&& m, Map&& map)
      {
        map.erase(message::get_path(m));
        return true;
      }

      template <typename Message, typename Map>
      bool handle_foreign(Message&&, Map&&)
      {
        return false;
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
    static constexpr bool apply(Store&& s, Message&& m)
    {
      return detail::message_dispatch<typename std::decay_t<Store>::path, Message>
        ::apply(
          detail::map_store_action_handler{},
          std::forward<Message>(m),
          std::forward<Store>(s).map
        );
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
