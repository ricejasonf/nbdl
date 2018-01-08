//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_UI_HELPER_MATCH_BRANCH_HPP
#define NBDL_FWD_UI_HELPER_MATCH_BRANCH_HPP

#include <nbdl/ui_spec.hpp>
#include <utility>

namespace nbdl::ui_helper
{
  template <typename BranchSpec>
  struct get_branch_index_fn
  {
    template <typename Store>
    std::size_t operator()(Store store) const;
  };

  template <typename BranchSpec>
  constexpr get_branch_index_fn<BranchSpec> get_branch_index{};

  template <typename BranchSpec>
  struct match_branch_index_fn
  {
    template <typename Fn>
    void operator()(std::size_t index, Fn&& fn) const;
  };

  template <typename BranchSpec>
  constexpr match_branch_index_fn<BranchSpec> match_branch_index{};

  template <typename BranchSpec>
  struct match_branch_fn
  {
    template <typename StoreOrIndex, typename Fn>
    void operator()(StoreOrIndex, Fn&&) const;
  };

  template <typename BranchSpec>
  constexpr match_branch_fn<BranchSpec> match_branch{};
}

#endif
