//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/List.hpp>
#include<mpdef/TreeNode.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  {
    constexpr auto xs = mpdef::make_list(
      hana::make_map(
        mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
        mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>)
      ),
      hana::make_map(
        mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>),
        mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>)
      )
    );
    BOOST_HANA_CONSTANT_ASSERT(xs == hana::reverse(xs));
  }
  {
    constexpr auto xs = mpdef::make_list(
      hana::int_c<0>,
      hana::int_c<1>,
      hana::int_c<2>,
      hana::int_c<3>,
      hana::int_c<4>,
      hana::int_c<5>
    );
    BOOST_HANA_CONSTANT_ASSERT(hana::at(xs, hana::int_c<3>) == hana::int_c<3>);
  }
  {
    constexpr auto xs = mpdef::make_list(
      hana::int_c<0>,
      hana::int_c<1>,
      hana::int_c<2>,
      hana::int_c<3>,
      hana::int_c<4>,
      hana::int_c<5>
    );
    BOOST_HANA_CONSTANT_ASSERT(hana::unpack(xs, hana::make_tuple)
      == hana::make_tuple(
        hana::int_c<0>,
        hana::int_c<1>,
        hana::int_c<2>,
        hana::int_c<3>,
        hana::int_c<4>,
        hana::int_c<5>
      )
    );
  }
  {
    constexpr auto xs1 = mpdef::make_list(
      hana::int_c<0>,
      hana::int_c<1>,
      hana::int_c<2>,
      hana::int_c<3>,
      hana::int_c<4>,
      hana::int_c<5>
    );

    constexpr auto xs = mpdef::make_list(xs1, xs1, xs1);

    BOOST_HANA_CONSTANT_ASSERT(hana::flatten(xs)
      == mpdef::make_list(
        hana::int_c<0>,
        hana::int_c<1>,
        hana::int_c<2>,
        hana::int_c<3>,
        hana::int_c<4>,
        hana::int_c<5>,
        hana::int_c<0>,
        hana::int_c<1>,
        hana::int_c<2>,
        hana::int_c<3>,
        hana::int_c<4>,
        hana::int_c<5>,
        hana::int_c<0>,
        hana::int_c<1>,
        hana::int_c<2>,
        hana::int_c<3>,
        hana::int_c<4>,
        hana::int_c<5>
      )
    );
  }
}
