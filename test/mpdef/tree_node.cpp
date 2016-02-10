//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/TreeNode.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  // Comparable
  {
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(
      mpdef::make_tree_node(hana::type_c<void>, 
        hana::make_map(
          mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
          mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>)
        )
      )
      ,
      mpdef::make_tree_node(hana::type_c<void>, 
        hana::make_map(
          mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>),
          mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>)
        )
      )
    ));
  }

  // Product
  {
    constexpr auto x = mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>);

    BOOST_HANA_CONSTANT_ASSERT(hana::equal(hana::first(x), hana::int_c<1>));
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(hana::second(x), hana::int_c<-1>));
  }
  {
    constexpr auto pred = hana::compose(hana::equal.to(hana::int_c<1>), hana::first);
    constexpr auto xs = hana::make_tuple(
      mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
      mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>)
    );
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(
      hana::filter(xs, pred),
      hana::make_tuple(mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>))
    ));
  }
}
