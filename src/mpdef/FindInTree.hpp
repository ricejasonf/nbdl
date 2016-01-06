//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_FIND_IN_TREE_HPP
#define NBDL_MPDEF_FIND_IN_TREE_HPP

#include<boost/hana.hpp>
#include<mpdef/CollectSettings.hpp>

namespace mpdef {

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
  constexpr auto helper_(Tree const& tree, AncestorSummary const& ancestor_summary, hana::true_) const
  {
    return hana::make_tuple(hana::make_pair(tree, ancestor_summary));
  }

  template<typename Tree, typename AncestorSummary>
  constexpr auto helper_(Tree const& tree, AncestorSummary const& ancestor_summary, hana::false_) const
  {
    auto new_summary = summarizeAncestry(ancestor_summary, tree);
    return hana::flatten(hana::unpack(hana::second(tree),
      hana::on(hana::make_tuple, hana::reverse_partial(*this, new_summary))));
  }

  template<typename Tree, typename AncestrySummary>
  constexpr auto helper(Tree const& tree, AncestrySummary const& ancestry_summary, hana::true_) const
  {
    return helper_(tree, ancestry_summary, pred(tree));
  }

  template<typename Leaf, typename AncestrySummary>
  constexpr auto helper(Leaf, AncestrySummary, hana::false_) const
  {
    return hana::make_tuple();
  }

  template<typename Tree, typename AncestrySummary>
  constexpr auto operator()(Tree const& tree, AncestrySummary const& ancestry_summary) const
  {
    return helper(tree, ancestry_summary,
      hana::type_c<hana::tag_of_t<decltype(hana::second(tree))>>
      ^hana::in^
      hana::make_tuple(
        hana::type_c<hana::tuple_tag>,
        hana::type_c<hana::map_tag>
      )
    );
  }
};

}//details

// returns ((node, ancestry_summary)...)
struct FindInTree
{
  template<typename Tree, typename InitialSummary, typename SummarizeAncestry, typename Pred>
  constexpr auto operator()(
      Tree const& tree,
      InitialSummary const& initial_summary,
      SummarizeAncestry const& summarizeAncestry,
      Pred const& pred) const
  {
    return details::InTreeFinder<Pred, SummarizeAncestry>(pred, summarizeAncestry)(
      tree,
      initial_summary
    );
  }

  //inherits settings from parents
  template<typename... Tag>
  constexpr auto withSettings(Tag&&... tag) const
  {
    auto initial_state = mpdef::withSettings(std::forward<Tag>(tag)...);
    /*
    return hana::curry<2>(hana::demux(mpdef::collectSettings)
      (
       //(tree, summarize, pred)
        hana::demux(hana::partial(hana::flip(*this), initial_state))
        (hana::arg<2>, hana::always(mpdef::collectSettings), hana::arg<1>)
      )
    );
    */
    return hana::curry<2>(hana::demux(hana::partial(hana::flip(*this), initial_state))
      (hana::arg<2>, hana::always(mpdef::collectSettings), hana::arg<1>));
  }
};
constexpr FindInTree findInTree{};
constexpr auto createInTreeFinder = hana::curry<4>(hana::demux(findInTree)
  (hana::arg<4>, hana::arg<3>, hana::arg<2>, hana::arg<1>));


}//mpdef
#endif
