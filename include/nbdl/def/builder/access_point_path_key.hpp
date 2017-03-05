//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ACCESS_POINT_PATH_KEY_HPP
#define NBDL_DEF_BUILDER_ACCESS_POINT_PATH_KEY_HPP

#include <nbdl/def/directives.hpp>

namespace nbdl_def::builder
{
  namespace hana = boost::hana;

  // Get the key type for an AccessPoint
  // Returns hana::type of the path_key
  struct access_point_path_key_fn
  {
    template <typename NodeChildren>
    constexpr auto operator()(NodeChildren node_children) const
    {
      constexpr auto result = hana::find(node_children, tag::PathKey);
      
      // TODO make concept KeyedEntity or something to specify
      // the PathKey for an entity if it isn't specified in the
      // AccessPoint
      static_assert(decltype(hana::is_just(result))::value,
        "No PathKey is specified for AccessPoint"
      //"Note that the user can opt to specialize "
      //"nbdl::entity_path_key instead. "
      );

      return result.value();
    }
  };

  constexpr access_point_path_key_fn access_point_path_key{};
}

#endif
