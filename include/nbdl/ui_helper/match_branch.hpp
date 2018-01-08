//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_MATCH_BRANCH_HPP
#define NBDL_UI_HELPER_MATCH_BRANCH_HPP

#include <nbdl/detail/match_if.hpp>
#include <nbdl/fwd/ui_helper/match_branch.hpp>
#include <nbdl/ui_helper/match_path_spec.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

namespace nbdl::ui_helper::detail
{
  using namespace boost::mp11;

  template <typename BranchSpec>
  struct get_branch_index_impl;

  template <typename PathSpec, typename Branches>
  struct get_branch_index_impl<ui_spec::branch_spec<ui_spec::match_tag, PathSpec, Branches>>
  {
    template <typename Store>
    static std::size_t apply(Store store)
    {
      using Types = mp_transform<mp_first, Branches>;
      std::size_t result = -1;

      match_path_spec(store, PathSpec{}, [&](auto const& value)
      {
        using T = std::decay_t<decltype(value)>;
        using Index = mp_min<mp_find<Types, T>, mp_find<Types, void>>;
        static_assert(Index::value < mp_size<Types>::value, "Matching branch not found");
        result = Index::value;
      });

      return result;
    }
  };

  template <typename PathSpec, typename Branches>
  struct get_branch_index_impl<ui_spec::branch_spec<ui_spec::match_if_tag, PathSpec, Branches>>
  {
    template <typename Store>
    static std::size_t apply(Store store)
    {
      std::size_t result = -1;

      match_path_spec(store, PathSpec{}, [&](auto const& value)
      {
        result = nbdl::detail::match_if(mp_transform<mp_first, Branches>{}, value);
      });

      return result;
    }
  };
}

namespace nbdl::ui_helper
{
  using namespace boost::mp11;

  template <typename BranchSpec>
  template <typename Store>
  std::size_t get_branch_index_fn<BranchSpec>::operator()(Store store) const
  {
    using Impl = detail::get_branch_index_impl<BranchSpec>;
    return Impl::apply(store);
  }

  template <typename BranchSpec>
  template <typename Fn>
  void match_branch_index_fn<BranchSpec>::operator()(std::size_t index, Fn&& fn) const
  {
    mp_with_index<mp_size<mp_third<BranchSpec>>>(index, std::forward<Fn>(fn));
  }

  template <typename BranchSpec>
  template <typename StoreOrIndex, typename Fn>
  void match_branch_fn<BranchSpec>::operator()(StoreOrIndex s, Fn&& fn) const
  {
    if constexpr(std::is_same<StoreOrIndex, std::size_t>::value)
    {
      match_branch_index<BranchSpec>(s, [&](auto index)
      {
        std::forward<Fn>(fn)(index, mp_second<mp_at<mp_third<BranchSpec>, decltype(index)>>{});
      });
    }
    else
    {
      std::size_t i = get_branch_index<BranchSpec>(s);
      match_branch_index<BranchSpec>(i, [&](auto index)
      {
        std::forward<Fn>(fn)(index, mp_second<mp_at<mp_third<BranchSpec>, decltype(index)>>{});
      });
    }
  }
}

#endif
