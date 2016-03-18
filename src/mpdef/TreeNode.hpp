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
struct tree_node { };

struct tree_node_tag;

struct make_tree_node_fn
{
  template<typename T, typename U>
  constexpr auto operator()(T, U) const
  {
    return mpdef::tree_node<T, U>{};
  }
};
constexpr make_tree_node_fn make_tree_node{};

struct is_tree_node_fn
{
  template <typename First, typename Second>
  constexpr auto operator()(mpdef::tree_node<First, Second>) const
  { return hana::true_c; }
  template <typename T>
  constexpr auto operator()(T) const
  { return hana::false_c; }
};
constexpr is_tree_node_fn is_tree_node{};

}//mpdef

namespace boost { namespace hana {

  // Product

  template<typename First, typename Second>
  struct first_impl<mpdef::tree_node<First, Second>>
  {
    static constexpr First apply(mpdef::tree_node<First, Second>) { return First{}; }
  };

  template<typename First, typename Second>
  struct second_impl<mpdef::tree_node<First, Second>>
  {
    static constexpr Second apply(mpdef::tree_node<First, Second>) { return Second{}; }
  };

  // Searchable - proxies search to Second

  template<typename First, typename Second>
  struct find_if_impl<mpdef::tree_node<First, Second>>
  {
    template<typename Pred>
    static constexpr auto apply(mpdef::tree_node<First, Second>, Pred const& pred)
    {
      return hana::find_if(Second{}, pred);
    }
  };

  template<typename First, typename Second>
  struct find_impl<mpdef::tree_node<First, Second>>
  {
    template<typename Tag>
    static constexpr auto apply(mpdef::tree_node<First, Second>, Tag)
    {
      return hana::find(Second{}, Tag{});
    }
  };

  template<typename First, typename Second>
  struct any_of_impl<mpdef::tree_node<First, Second>>
  {
    template<typename Pred>
    static constexpr auto apply(mpdef::tree_node<First, Second>, Pred const& pred)
    {
      return hana::any_of(Second{}, pred);
    }
  };

}} // boost::hana
#endif
