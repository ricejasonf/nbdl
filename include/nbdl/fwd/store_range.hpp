//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_STORE_RANGE_HPP
#define NBDL_FWD_STORE_RANGE_HPP

namespace nbdl
{
  // store_iterator

  struct store_iterator_tag { };

  template <typename Key, typename Iterator, typename Store>
  struct store_iterator
  {
    using hana_tag = store_iterator_tag;

    Iterator itr_pos;
    Store store;

    using store_key_t = Key;

    inline void operator++() { ++itr_pos; }

    // The contained iterator is accessed via nbdl::match(*this, key, fn)
    inline store_iterator& operator*() { return *this; }

    inline bool operator==(store_iterator const& x) const
    { return itr_pos == x.itr_pos; }

    inline bool operator!=(store_iterator const& x) const
    { return itr_pos != x.itr_pos; }
  };

  // store_range

  template <typename Key, typename Container, typename Store>
  struct store_range_t
  {
    Container& container;
    Store store;

    auto begin()  const noexcept;
    auto end()    const noexcept;
    auto begin()        noexcept;
    auto end()          noexcept;
  };

  struct store_range_fn
  {
    template <typename KeyType, typename Container, typename Store>
    auto operator()(KeyType, Container&, Store) const;
  };

  constexpr store_range_fn store_range{};
}

#endif
