//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_CREATE_TYPE_SETTING_GETTER_HPP
#define NBDL_MPDEF_CREATE_TYPE_SETTING_GETTER_HPP

#include<boost/hana.hpp>
#include "../Make.hpp"
#include "../def/meta.hpp" //FindSetting.hpp

namespace nbdl_def_meta {
namespace details {

template<typename Pred, typename SummarizeAncestry>
struct TreeFinder
{
  Pred const& pred;
  SummarizeAncestry const& summarizeAncestry;

  TreeFinder(Pred const& p, SummarizeAncestry const& s) :
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
    return hana::unpack(hana::second(tree),
      [](auto... child) {
        return (*this)(child, new_summary);
      });
  }

  template<typename Tree, typename AncestrySummary>
  auto operator()(Tree const& tree, AncestrySummary const& ancestry_summary)
  {
    return helper(tree, ancestry_summary, pred(tree));
  }
};

}//details

//returns ((node, ancestry_summary)...)
struct FindTree
{
  template<typename Tree, typename Pred, typename SummarizeAncestry>
  constexpr auto operator()(
      Tree const& tree,
      Pred const& pred,
      SummarizeAncestry const& summarizeAncestry)
  {
    return decltype(
      details::TreeFinder<Pred, SummarizeAncestry>(pred, summarizeAncestry)(
        hana::tuple<>{},
        tree
      )
    ){};
  }
};
constexpr FindTree findTree{};
constexpr auto createTreeFinder = hana::curry<2>(hana::flip(findTree));

}//nbdl_def_meta
