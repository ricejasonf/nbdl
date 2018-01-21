//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAP_STORE_HPP
#define NBDL_FWD_MAP_STORE_HPP

#include <nbdl/tags.hpp>
#include <nbdl/variant.hpp>

namespace nbdl
{
  struct basic_map_store_tag { };

  template <typename Key
          , typename Value
          , typename NotInSetTag = nbdl::not_in_set
          , typename Tag = basic_map_store_tag>
  struct basic_map_store;

  struct map_store_tag { };

  template <typename Path, typename Entity>
  using map_store = basic_map_store<
    Path
  , nbdl::detail::variant<nbdl::unresolved, Entity, nbdl::not_found>
  , nbdl::trigger_read
  , map_store_tag
  >;
}

#endif
