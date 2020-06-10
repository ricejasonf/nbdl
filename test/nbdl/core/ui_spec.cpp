//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/ui_spec.hpp>

#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <boost/hana.hpp>

namespace hana = boost::hana;

#define CHECK_TYPE_EQUAL(RESULT, EXPECTED) BOOST_HANA_CONSTANT_CHECK(hana::equal( \
  hana::typeid_(RESULT), hana::typeid_(EXPECTED) \
));

namespace
{
  template <int>
  struct skey_t { };

  template <int i>
  constexpr skey_t<i> key{};

  template <int i>
  struct some_type { };
};

int main()
{
  using namespace nbdl::ui_spec;

  // Get a path to a value from the root store.
  {
    {
      constexpr auto result = get(key<0>);
      constexpr auto expected = path_t<get_t<skey_t<0>>>{};

      CHECK_TYPE_EQUAL(result, expected);
    }
    {
      constexpr auto result = get(key<0>, key<1>);
      constexpr auto expected = path_t<get_t<skey_t<0>, skey_t<1>>>{};

      CHECK_TYPE_EQUAL(result, expected);
    }
  }
}
