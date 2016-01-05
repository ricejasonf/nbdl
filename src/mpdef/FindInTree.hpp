//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_FIND_IN_TREE_HPP
#define NBDL_MPDEF_FIND_IN_TREE_HPP

#include<boost/hana.hpp>
#include "../def/meta.hpp"

namespace nbdl_def_meta {

namespace hana = boost::hana;

namespace details {

template<typename Pred, typename SummarizeAncestry>
struct InTreeFinder
{
  Pred const& pred;
  SummarizeAncestry const& summarizeAncestry;

  constexpr InTreeFinder(Pred const& p, SummarizeAncestry const& s) :
    pred(p),
    summarizeAncestry(s)
  {}

  template<typename Tree, typename AncestorSummary>
  constexpr auto helper(Tree const& tree, AncestorSummary const& ancestor_summary, hana::true_)
  {
    return hana::make_tuple(hana::make_pair(tree, ancestor_summary));
  }

  template<typename Tree, typename AncestorSummary>
  constexpr auto helper(Tree const& tree, AncestorSummary const& ancestor_summary, hana::false_)
  {
    auto new_summary = summarizeAncestry(ancestor_summary, tree);
    return hana::flatten(hana::unpack(hana::second(tree),
      hana::on(hana::make_tuple, hana::reverse_partial(*this, new_summary))));
  }

  template<typename Tree, typename AncestrySummary>
  constexpr auto operator()(Tree const& tree, AncestrySummary const& ancestry_summary)
  {
    return helper(tree, ancestry_summary, pred(tree));
  }
};

}//details

//returns ((node, ancestry_summary)...)
struct FindInTree
{
  template<typename Pred, typename SummarizeAncestry, typename InitialSummary, typename Tree>
  constexpr auto operator()(
      Pred const& pred,
      SummarizeAncestry const& summarizeAncestry,
      InitialSummary const& initial_summary,
      Tree const& tree)
  {
    return details::InTreeFinder<Pred, SummarizeAncestry>(pred, summarizeAncestry)(
      tree,
      initial_summary
    );
  }
};
constexpr FindInTree findInTree{};
constexpr auto createInTreeFinder = hana::curry<4>(findInTree);

}//nbdl_def_meta
#endif
