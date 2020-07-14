//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STORE_RANGE_HPP
#define NBDL_STORE_RANGE_HPP

#include <nbdl/apply_action.hpp>
#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/store_range.hpp>
#include <nbdl/match.hpp>

#include <type_traits>

namespace nbdl
{
  template <typename Key, typename Container, typename Store>
  auto store_range_t<Key, Container, Store>::begin() const noexcept
  {
    using Iterator = decltype(container.begin());
    return store_iterator<Key, Iterator, Store>{container.begin(), store};
  }

  template <typename Key, typename Container, typename Store>
  auto store_range_t<Key, Container, Store>::end() const noexcept
  {
    using Iterator = decltype(container.end());
    return store_iterator<Key, Iterator, Store>{container.end(), store};
  }

  template <typename Key, typename Container, typename Store>
  auto store_range_t<Key, Container, Store>::begin() noexcept
  {
    using Iterator = decltype(container.begin());
    return store_iterator<Key, Iterator, Store>{container.begin(), store};
  }

  template <typename Key, typename Container, typename Store>
  auto store_range_t<Key, Container, Store>::end() noexcept
  {
    using Iterator = decltype(container.end());
    return store_iterator<Key, Iterator, Store>{container.end(), store};
  }

  template <typename Key, Container Container, Store Store>
  auto store_range_fn::operator()(Key, Container& c, Store s) const {
    return store_range_t<Key, Container, Store>{c, s};
  }
}

namespace nbdl
{
  template <>
  struct apply_action_impl<store_iterator_tag>
  {
    template <typename Store, typename Action>
    static constexpr auto apply(Store& s, Action&& a)
    {
      if constexpr(hana::is_a<store_iterator_action_tag, Action>
      )
      {
        return nbdl::apply_action(*(s.itr_pos), std::forward<Action>(a).value);
      }
      else
      {
        return nbdl::apply_action(s.store, std::forward<Action>(a));
      }
    }
  };

  template <>
  struct match_impl<store_iterator_tag>
  {
    template <typename Store, typename Key, typename Fn>
    static void apply(Store& store, Key&& key, Fn&& fn)
    {
      if constexpr(std::is_same<
          std::decay_t<Key>
        , typename std::decay_t<Store>::store_key_t
        >::value)
      {
        nbdl::match(*(store.itr_pos), std::forward<Fn>(fn));
      }
      else
      {
        nbdl::match(store.store, std::forward<Key>(key), std::forward<Fn>(fn));
      }
    }
  };
}

#endif
