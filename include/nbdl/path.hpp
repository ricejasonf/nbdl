//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

namespace nbdl
{
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
    template <typename ...Xs, typename ...Ys>
    constexpr auto operator()(path_t<Xs...>, Ys...) const
      -> path_t<Xs..., Ys...>
    { return {}; }
  };

  constexpr path_fn path{};

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

#endif
