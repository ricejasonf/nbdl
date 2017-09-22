//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_GROUP_BY_HPP
#define NBDL_MPDEF_GROUP_BY_HPP

#include <mpdef/list.hpp>
#include <mpdef/map.hpp>
#include <mpdef/pair.hpp>

#include <boost/mp11.hpp>
#include <type_traits>

namespace mpdef
{
  using namespace boost::mp11;

  namespace group_by_detail
  {
    template <typename P1, typename P2>
    using join_map_pair = mp_eval_if<
      std::is_same<void, P1>
    , P2
    , mp_push_back, P1, mp_second<P2>
    >;

    template <typename M, typename P>
    using add_to_map = mp_map_replace<M, join_map_pair<mp_map_find<M, mp_first<P>>, P>>;

    template <typename L>
    using group_by_impl = mp_fold<L, mpdef::list<>, add_to_map>;

    template <typename K, typename ...L>
    using to_group_pair_impl = mpdef::pair<K, mp_append<L...>>;

    template <typename P>
    using to_group_pair = mp_apply<to_group_pair_impl, P>;
  }

  // group_by
  // Input a list of pair lists with values of lists
  // It joins the lists by keys yielding an mpdef::map
  template <typename L>
  using group_by = mp_rename<
    mp_transform<
      group_by_detail::to_group_pair
    , group_by_detail::group_by_impl<L>
    >
  , mpdef::map
  >;
}

#endif
