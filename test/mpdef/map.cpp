//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/map.hpp>
#include <mpdef/tree_node.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  {
    constexpr auto m = mpdef::make_map(
      mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
      mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>),
      mpdef::make_tree_node(hana::int_c<3>, hana::int_c<-3>)
    );

    BOOST_HANA_CONSTANT_ASSERT(hana::at_key(m, hana::int_c<1>) == hana::int_c<-1>);
    BOOST_HANA_CONSTANT_ASSERT(hana::at_key(m, hana::int_c<2>) == hana::int_c<-2>);
    BOOST_HANA_CONSTANT_ASSERT(hana::at_key(m, hana::int_c<3>) == hana::int_c<-3>);

    BOOST_HANA_CONSTANT_ASSERT(m[hana::int_c<1>] == hana::int_c<-1>);
    BOOST_HANA_CONSTANT_ASSERT(m[hana::int_c<2>] == hana::int_c<-2>);
    BOOST_HANA_CONSTANT_ASSERT(m[hana::int_c<3>] == hana::int_c<-3>);

    BOOST_HANA_CONSTANT_ASSERT(hana::find(m, hana::int_c<1>) == hana::just(hana::int_c<-1>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find(m, hana::int_c<2>) == hana::just(hana::int_c<-2>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find(m, hana::int_c<3>) == hana::just(hana::int_c<-3>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find(m, hana::int_c<4>) == hana::nothing);

    BOOST_HANA_CONSTANT_ASSERT(hana::find_if(m, hana::equal.to(hana::int_c<1>))
      == hana::just(hana::int_c<-1>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find_if(m, hana::equal.to(hana::int_c<2>))
      == hana::just(hana::int_c<-2>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find_if(m, hana::equal.to(hana::int_c<3>))
      == hana::just(hana::int_c<-3>));
    BOOST_HANA_CONSTANT_ASSERT(hana::find_if(m, hana::equal.to(hana::int_c<4>))
      == hana::nothing);

    BOOST_HANA_CONSTANT_ASSERT(hana::any_of(m, hana::equal.to(hana::int_c<1>)));
    BOOST_HANA_CONSTANT_ASSERT(hana::any_of(m, hana::equal.to(hana::int_c<2>)));
    BOOST_HANA_CONSTANT_ASSERT(hana::any_of(m, hana::equal.to(hana::int_c<3>)));
    BOOST_HANA_CONSTANT_ASSERT(hana::not_(hana::any_of(m, hana::equal.to(hana::int_c<4>))));

    BOOST_HANA_CONSTANT_ASSERT(hana::contains(m, hana::int_c<1>));
    BOOST_HANA_CONSTANT_ASSERT(hana::contains(m, hana::int_c<2>));
    BOOST_HANA_CONSTANT_ASSERT(hana::contains(m, hana::int_c<3>));
    BOOST_HANA_CONSTANT_ASSERT(hana::not_(hana::contains(m, hana::int_c<4>)));
  }
  {
    constexpr auto a = mpdef::make_map(
      mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
      mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>)
    );
    constexpr auto b = mpdef::make_map(
      mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>),
      mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>)
    );
    constexpr auto c = mpdef::make_map(
      mpdef::make_tree_node(hana::int_c<2>, hana::int_c<-2>),
      mpdef::make_tree_node(hana::int_c<1>, hana::int_c<-1>)
    );

    BOOST_HANA_CONSTANT_ASSERT(a == b);
    BOOST_HANA_CONSTANT_ASSERT(a == c);
  }
}
