//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/JustifyType.hpp>

#include<boost/hana.hpp>
#include<type_traits>

namespace hana = boost::hana;

int main()
{
  {
    // I used void to show that we aren't instantiating anything.
    // (An optional<void> doesn't really make sense though.)
    BOOST_HANA_CONSTANT_ASSERT(
      mpdef::justify_type(hana::decltype_(hana::just(hana::type_c<void>)))
        ==
      hana::type_c<hana::optional<void>>
    );

    BOOST_HANA_CONSTANT_ASSERT(
      mpdef::justify_type(hana::decltype_(hana::optional<>{}))
        ==
      hana::type_c<decltype(hana::nothing)>
    );
  }
  {
    // test `const hana::optional<int>`
    constexpr auto x = hana::just(hana::type_c<int>);
    BOOST_HANA_CONSTANT_ASSERT(
      mpdef::justify_type(hana::decltype_(x))
        ==
      hana::type_c<hana::optional<int>>
    );

    // test `const hana::optional<>`
    BOOST_HANA_CONSTANT_ASSERT(
      mpdef::justify_type(hana::decltype_(hana::nothing))
        ==
      hana::type_c<decltype(hana::nothing)>
    );
  }

}

