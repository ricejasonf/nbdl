//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_FIND_IN_TREE_HPP
#define NBDL_MPDEF_FIND_IN_TREE_HPP

#include<mpdef/CollectSettings.hpp>
#include<mpdef/List.hpp>
#include<mpdef/TreeNode.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace mpdef {

namespace hana = boost::hana;

namespace details {

// isLeaf
template<typename T>
constexpr auto isLeaf(T)
{
  return hana::true_c;
}
template<typename First, typename ...X>
constexpr auto isLeaf(mpdef::TreeNode<First, mpdef::List<X...>>)
{
  return hana::false_c;
}
template<typename First, typename ...X>
constexpr auto isLeaf(mpdef::TreeNode<First, hana::map<X...>>)
{
  return hana::false_c;
}

// InTreeFinder
template<typename Pred, typename Summarize>
struct InTreeFinder
{
  Pred const& pred;
  Summarize const& summarize;

  constexpr InTreeFinder(Pred const& p, Summarize const& s) :
    pred(p),
    summarize(s)
  {}

  template<typename Tree, typename Summary, typename IsLeaf>
  constexpr auto helper(
    Tree const& tree,
    Summary const& summary,
    hana::true_, //pred
    IsLeaf       //could be branch or leaf
    ) const
  {
    //matching node
    return mpdef::List<decltype(mpdef::make_tree_node(tree, summarize(summary, tree)))>{};
  }

  template<typename Leaf, typename Summary>
  constexpr auto helper(
    Leaf,
    Summary,
    hana::false_, //pred
    hana::true_   //is_leaf
    ) const
  {
    return mpdef::List<>{};
  }

  template<typename Tree, typename Summary>
  constexpr auto helper(
    Tree const& tree,
    Summary const& summary,
    hana::false_, //pred
    hana::false_  //is_leaf
    ) const
  {
    auto new_summary = summarize(summary, tree);
    return hana::flatten(hana::unpack(hana::second(tree),
      hana::on(mpdef::make_list, hana::reverse_partial(*this, std::move(new_summary)))));
  }

  template<typename Tree, typename Summary>
  constexpr auto operator()(Tree const& tree, Summary const& summary) const
  {
    return helper(tree, summary, pred(tree), details::isLeaf(tree));
  }
};

}//details

// returns ((node, summary)...)
struct FindInTree
{
  template<typename Tree, typename InitialSummary, typename Summarize, typename Pred>
  constexpr auto operator()(
      Tree const& tree,
      InitialSummary const& initial_summary,
      Summarize const& summarize,
      Pred const& pred) const
  {
    return details::InTreeFinder<Pred, Summarize>(pred, summarize)(
      tree,
      initial_summary
    );
  }

  //inherits settings from parents
  template<typename... Tag>
  constexpr auto withSettings(Tag&&... tag) const
  {
    auto initial_state = mpdef::withSettings(std::forward<Tag>(tag)...);
    return hana::curry<2>( 
      hana::demux(hana::partial(hana::flip(*this), std::move(initial_state)))
      (hana::arg<2>, hana::always(mpdef::collectSettings), hana::arg<1>)
    );
  }
};
constexpr FindInTree findInTree{};
constexpr auto createInTreeFinder = hana::curry<4>(hana::demux(findInTree)
  (hana::arg<4>, hana::arg<3>, hana::arg<2>, hana::arg<1>));


}//mpdef
#endif
