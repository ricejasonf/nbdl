//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_KEY_HPP
#define NBDL_KEY_HPP

#include <cstdint>
#include <utility>

namespace nbdl
{
  // Type safe representation for an entity key
  template <typename T, typename Key = uint32_t>
  struct key_impl
  {
    Key value;

    key_impl() = delete;
  };

  template <typename T, typename Key = uint32_t>
  using key = key_impl<std::decay_t<T>, Key>;

  template <typename T, typename Key>
  bool operator==(key_impl<T, Key> const& x, key_impl<T, Key> const& y)
  {
    return x.value == y.value;
  }
}

#endif
