//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<boost/hana.hpp>
#include<mpdef/FindInTree.hpp>
#include<def/meta.hpp>

namespace hana = boost::hana;

MPDEF_DIRECTIVE(FindMe);
MPDEF_DIRECTIVE(Root);
MPDEF_DIRECTIVE(A);
MPDEF_DIRECTIVE(B);

MPDEF_DIRECTIVE_LIST(Widgets);

MPDEF_DIRECTIVE_LEAF(Setting1);
MPDEF_DIRECTIVE_LEAF(Setting2);
MPDEF_DIRECTIVE_LEAF(Setting3);
MPDEF_DIRECTIVE_LEAF(Setting4);

int main()
{
  {
    constexpr auto tree =
      Root(
        FindMe(A()),
        A(
          FindMe(),
          B(FindMe()),
          FindMe(B())
        ),
        FindMe(),
        FindMe(B()),
        Widgets(
          A(),
          A(),
          A(FindMe()),
          A()
        )
      );

    constexpr auto countDepth = hana::demux(hana::partial(hana::plus, hana::int_c<1>))(hana::arg<1>);
    constexpr auto pred = hana::compose(hana::equal.to(tag::FindMe), hana::first);
    constexpr auto x = mpdef::createInTreeFinder(pred, countDepth, hana::int_c<0>)(tree);
    constexpr auto y = hana::make_tuple(
      hana::make_pair(FindMe(A()) , hana::int_c<2>),
      hana::make_pair(FindMe()    , hana::int_c<3>),
      hana::make_pair(FindMe()    , hana::int_c<4>),
      hana::make_pair(FindMe(B()) , hana::int_c<3>),
      hana::make_pair(FindMe()    , hana::int_c<2>),
      hana::make_pair(FindMe(B()) , hana::int_c<2>),
      hana::make_pair(FindMe()    , hana::int_c<4>)
    );
    BOOST_HANA_CONSTANT_ASSERT(x == y);
  }

  // use withSettings to collect settings
  {
    constexpr auto tree =
      Root(
        Setting1(hana::int_c<42>),
        FindMe(),
        A(
          Setting2(hana::int_c<256>),
          FindMe(),
          B(
            Setting3(hana::int_c<512>),
            FindMe()
          )
        ),
        B(
          Setting1(hana::int_c<84>),
          FindMe(),
          A(
            Setting1(hana::int_c<1024>),
            Setting2(hana::int_c<768>),
            FindMe(Setting3(hana::int_c<1080>))
          )
        ),
        FindMe(
          Setting1(hana::int_c<1>),
          Setting4(hana::int_c<4>)
        )
      );

    constexpr auto pred = hana::compose(hana::equal.to(tag::FindMe), hana::first);
    auto x = mpdef::findInTree
      .withSettings(tag::Setting1, tag::Setting2, tag::Setting3, tag::Setting4)
      (pred)(tree);
    constexpr auto y = hana::make_tuple(

      hana::make_pair(FindMe(), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<42>)),
        hana::make_pair(tag::Setting2, hana::nothing),
        hana::make_pair(tag::Setting3, hana::nothing),
        hana::make_pair(tag::Setting4, hana::nothing)
      )),

      hana::make_pair(FindMe(), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<42>)),
        hana::make_pair(tag::Setting2, hana::just(hana::int_c<256>)),
        hana::make_pair(tag::Setting3, hana::nothing),
        hana::make_pair(tag::Setting4, hana::nothing)
      )),

      hana::make_pair(FindMe(), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<42>)),
        hana::make_pair(tag::Setting2, hana::just(hana::int_c<256>)),
        hana::make_pair(tag::Setting3, hana::just(hana::int_c<512>)),
        hana::make_pair(tag::Setting4, hana::nothing)
      )),

      hana::make_pair(FindMe(), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<84>)),
        hana::make_pair(tag::Setting2, hana::nothing),
        hana::make_pair(tag::Setting3, hana::nothing),
        hana::make_pair(tag::Setting4, hana::nothing)
      )),

      hana::make_pair(FindMe(Setting3(hana::int_c<1080>)), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<1024>)),
        hana::make_pair(tag::Setting2, hana::just(hana::int_c<768>)),
        hana::make_pair(tag::Setting3, hana::just(hana::int_c<1080>)),
        hana::make_pair(tag::Setting4, hana::nothing)
      )),

      hana::make_pair(FindMe(
          Setting1(hana::int_c<1>),
          Setting4(hana::int_c<4>)
        ), hana::make_map(
        hana::make_pair(tag::Setting1, hana::just(hana::int_c<1>)),
        hana::make_pair(tag::Setting2, hana::nothing),
        hana::make_pair(tag::Setting3, hana::nothing),
        hana::make_pair(tag::Setting4, hana::just(hana::int_c<4>))
      ))

    );
    BOOST_HANA_CONSTANT_ASSERT(x == y);
  }

  // find leafs
  {
    constexpr hana::int_<1> foo{};
    constexpr auto tree =
      Root(
        Setting1(foo),
        Setting2(foo),
        A(
          Setting1(foo),
          B(Setting1(foo))
        ),
        Widgets(
          A(),
          Setting1(foo),
          Setting1(foo),
          A(Setting1(foo)),
          A()
        )
      );

    constexpr auto countDepth = hana::demux(hana::partial(hana::plus, hana::int_c<1>))(hana::arg<1>);
    constexpr auto pred = hana::compose(hana::equal.to(tag::Setting1), hana::first);
    constexpr auto x = mpdef::createInTreeFinder(pred, countDepth, hana::int_c<0>)(tree);
    constexpr auto y = hana::make_tuple(
      hana::make_pair(Setting1(foo), hana::int_c<2>),
      hana::make_pair(Setting1(foo), hana::int_c<3>),
      hana::make_pair(Setting1(foo), hana::int_c<4>),
      hana::make_pair(Setting1(foo), hana::int_c<3>),
      hana::make_pair(Setting1(foo), hana::int_c<3>),
      hana::make_pair(Setting1(foo), hana::int_c<4>)
    );
    BOOST_HANA_CONSTANT_ASSERT(x == y);
  }
}