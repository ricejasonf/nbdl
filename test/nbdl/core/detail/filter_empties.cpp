//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/filter_empties.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

TEST_CASE("Filter the empties.", "[detail][filter_empties]")
{
  auto xs = hana::make_tuple(
    42
  , 5.04f
  , hana::int_c<42>
  , hana::make_tuple()
  , std::string("foo")
  , hana::make_tuple()
  );

  auto expected = hana::make_tuple(
    42
  , 5.04f
  , std::string("foo")
  );

  {
    auto result = hana::unpack(
      xs
    , nbdl::detail::filter_empties_fn<decltype(hana::make_tuple)>{hana::make_tuple}
    );

    CHECK((result == expected));
  }

  {
    auto result = hana::unpack(
      std::move(xs)
    , nbdl::detail::filter_empties_fn<decltype(hana::make_tuple)>{hana::make_tuple}
    );

    CHECK((result == expected));
  }

  {
    auto result = hana::unpack(
      hana::make_tuple(
        42
      , 5.04f
      , hana::int_c<42>
      , hana::make_tuple()
      , std::string("foo")
      , hana::make_tuple()
      )
    , nbdl::detail::filter_empties_fn<decltype(hana::make_tuple)>{hana::make_tuple}
    );

    CHECK((result == expected));
  }
  {
    using Xs = decltype(xs);
    using result = decltype(hana::unpack(
      std::declval<Xs>()
    , nbdl::detail::filter_empties_fn<decltype(hana::make_tuple)>{hana::make_tuple}
    ));

    CHECK((hana::type_c<result> == hana::typeid_(expected)));
  }
}
