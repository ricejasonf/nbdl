//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_TREE_NODE_HPP
#define NBDL_MPDEF_TREE_NODE_HPP

#include<boost/hana.hpp>
#include<type_traits>
#include<utility>

namespace mpdef {

namespace hana = boost::hana;

template<typename First, typename Second>
struct TreeNode { };

struct TreeNodeTag;

struct MakeTreeNode
{
  template<typename T, typename U>
  constexpr auto operator()(T, U) const
  {
    return mpdef::TreeNode<T, U>{};
  }
};
constexpr MakeTreeNode make_tree_node{};

struct IsTreeNode
{
  template <typename First, typename Second>
  constexpr auto operator()(mpdef::TreeNode<First, Second>) const
  { return hana::true_c; }
  template <typename T>
  constexpr auto operator()(T) const
  { return hana::false_c; }
};
constexpr IsTreeNode is_tree_node{};

}//mpdef

namespace boost { namespace hana {

  // Product

  template<typename First, typename Second>
  struct first_impl<mpdef::TreeNode<First, Second>>
  {
    static constexpr First apply(mpdef::TreeNode<First, Second>) { return First{}; }
  };

  template<typename First, typename Second>
  struct second_impl<mpdef::TreeNode<First, Second>>
  {
    static constexpr Second apply(mpdef::TreeNode<First, Second>) { return Second{}; }
  };

  // Searchable - proxies search to Second

  template<typename First, typename Second>
  struct find_if_impl<mpdef::TreeNode<First, Second>>
  {
    template<typename Pred>
    static constexpr auto apply(mpdef::TreeNode<First, Second>, Pred const& pred)
    {
      return hana::find_if(Second{}, pred);
    }
  };

  template<typename First, typename Second>
  struct find_impl<mpdef::TreeNode<First, Second>>
  {
    template<typename Tag>
    static constexpr auto apply(mpdef::TreeNode<First, Second>, Tag)
    {
      return hana::find(Second{}, Tag{});
    }
  };

  template<typename First, typename Second>
  struct any_of_impl<mpdef::TreeNode<First, Second>>
  {
    template<typename Pred>
    static constexpr auto apply(mpdef::TreeNode<First, Second>, Pred const& pred)
    {
      return hana::any_of(Second{}, pred);
    }
  };

}} // boost::hana
#endif
