//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXT_STD_UNORDERED_MAP
#define NBDL_EXT_STD_UNORDERED_MAP

#include <nbdl/match.hpp>

#include <unordered_map>

namespace nbdl
{
  // This could be written to work for any AssociativeContainer
  // that has unique keys, but the only useful one is unordered_map.
  template <typename ...MapArgs>
  struct match_impl<std::unordered_map<MapArgs...>>
  {
    using T = std::unordered_map<MapArgs...>;

    template <typename Store, typename Fn>
    static void apply(Store&& s, Fn&& fn)
    {
      std::forward<Fn>(fn)(std::forward<Store>(s));
    }

    template <typename Store, typename Key, typename Fn>
    static void apply(Store&& s, Key&& k, Fn&& fn)
    {
      auto itr = s.find(std::forward<Key>(k));
      if (itr == s.end())
      {
        std::forward<Fn>(fn)(nbdl::uninitialized{});
      }
      else
      {
        std::forward<Fn>(fn)(itr->second);
      }
    }
  };
}

#endif
