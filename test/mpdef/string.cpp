//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/string.hpp>

#include <boost/hana/string.hpp>

namespace hana = boost::hana;

int main()
{
  // string_concat
  {
    auto result = mpdef::string_concat(BOOST_HANA_STRING("foo "), BOOST_HANA_STRING("bar"));
    (void) result;
    auto expected = BOOST_HANA_STRING("foo bar");
    (void) expected;
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
  }

  // string_join
  {
    auto result = mpdef::string_join(hana::string<' '>{},
                                     BOOST_HANA_STRING("foo"),
                                     BOOST_HANA_STRING("bar")
                                    );
    (void) result;
    auto expected = BOOST_HANA_STRING("foo bar");
    (void) expected;
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
  }

  // string_join (partially applied fn)
  {
    constexpr auto joiner = mpdef::string_join(hana::string<' '>{});
    auto result = joiner(BOOST_HANA_STRING("foo"),
                         BOOST_HANA_STRING("bar")
                        );
    (void) result;
    auto expected = BOOST_HANA_STRING("foo bar");
    (void) expected;
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
  }
}
