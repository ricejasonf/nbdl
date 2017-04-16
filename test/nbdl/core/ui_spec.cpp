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
  struct key_t { };

  template <int i>
  constexpr key_t<i> key{};

  template <int>
  struct spec_t { };

  template <int i>
  constexpr spec_t<i> spec{};

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
      constexpr auto expected = path_t<get_t<key_t<0>>>{};

      CHECK_TYPE_EQUAL(result, expected);
    }
    {
      constexpr auto result = get(key<0>, key<1>);
      constexpr auto expected = path_t<get_t<key_t<0>, key_t<1>>>{};

      CHECK_TYPE_EQUAL(result, expected);
    }
  }

  // match - Create a spec of branches on
  // different types in a Store at a given path.
  {
    auto spec_1 = [](auto&&) { return spec<1>; };
    auto spec_2 = [](auto&&) { return spec<2>; };

    constexpr auto result = match(
      get(key<0>, key<1>)
    , when<some_type<0>>(spec<0>)
    , when<some_type<1>>(spec_1)
    , when<>(spec_2)
    );

    constexpr auto expected = match_t<
      path_t<get_t<key_t<0>, key_t<1>>>
    , mpdef::list<
        mpdef::pair<some_type<0>, spec_t<0>>
      , mpdef::pair<some_type<1>, spec_t<1>>
      , mpdef::pair<void, spec_t<2>>
      >
    >{};

    CHECK_TYPE_EQUAL(result, expected);
  }

  // make_path_match_type
  {
    auto spec_1 = [](auto&&) { return spec<2>; };
    constexpr auto path1 = get(key<0>, key<1>);

    constexpr auto result = make_path_match_type<
      when_t<some_type<0>, decltype(spec_1)>
    , decltype(path1)
    >::type{};

    constexpr auto expected = path_t<
      match_type<some_type<0>, key_t<0>, key_t<1>>
    >{};

    CHECK_TYPE_EQUAL(result, expected);
  }
}
