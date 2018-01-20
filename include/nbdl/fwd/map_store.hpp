//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAP_STORE_HPP
#define NBDL_FWD_MAP_STORE_HPP

#include <nbdl/match.hpp>
#include <nbdl/variant.hpp>

namespace nbdl
{
  struct basic_map_store_tag { };
  struct map_store_tag { };

  template <typename Key, typename Value, typename Tag = basic_map_store_tag>
  struct basic_map_store;

  template <typename Path, typename Entity>
  using map_store = basic_map_store<Path, nbdl::variant<Entity, nbdl::not_found>, map_store_tag>;
}

#endif
