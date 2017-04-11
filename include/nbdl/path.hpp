//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

#include <boost/hana/core/tag_of.hpp>

namespace nbdl
{
  struct path_tag { };

  template <typename ...Xs>
  struct path_t
  { };

  struct root_path_fn
  {
    template <typename ...Xs>
    constexpr auto operator()(Xs...) const
      -> path_t<Xs...>
    { return {}; }
  };

  constexpr root_path_fn root_path{};

  struct path_fn
  {
    constexpr auto operator()() const
      -> path_t<>
    { return {}; }

    template <typename ...Xs, typename ...Ys>
    constexpr auto operator()(path_t<Xs...>, Ys...) const
      -> path_t<Xs..., Ys...>
    { return {}; }
#if 0
    template <typename ...Xs, typename ...Ys>
    constexpr auto operator()(matched_t<Xs...>, Ys...) const
      -> path_t<matched_t<Xs...>, Ys...>
    { return {}; }
#endif
  };

  constexpr path_fn path{};

  /*
   * Everything that is not a "path_node" is a leaf.
   */
  struct path_node_tag { };

  template <typename Key, typename PathToValue>
  struct path_node_t
  { };

  template <typename Key>
  struct path_node_fn
  {
    template <typename PathToValue>
    constexpr auto operator()(PathToValue) const
      -> path_node_t<Key, PathToValue>
    { return {}; }
  };

  template <typename Key>
  constexpr path_node_fn<Key> path_node{};
}

namespace boost::hana
{
  template<typename ...T>
  struct tag_of<nbdl::path_t<T...>> { using type = nbdl::path_tag; };

  template<typename T, typename U>
  struct tag_of<nbdl::path_node_t<T, U>> { using type = nbdl::path_node_tag; };
}

#endif
