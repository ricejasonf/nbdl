//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/concept/extras.hpp>
#include <nbdl/variant.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <catch.hpp>
#include <string>
#include <utility>

#include <iostream>

namespace foo {

// Note that nbdl::unresolved is the monostate type.
using my_variant = nbdl::variant<int, std::string>;

heavy_scheme {
  (import (nbdl spec))
  (context 'context (member) (arg1 arg2 arg3)
    (member '.foo 'int 42)
    (member '.bar 'std::string
           "initial string value for bar")
    (member '.baz 'int arg1)
    (member '.boo "std::pair<std::string, int>" arg2 arg3)
    (member '.my_var 'my_variant arg2))

  ; // get values initialized with literals
  (match-params-fn 'match_0 (store fn)
    (define root-path (get store))
    (fn
      (get store '.foo)
      (get root-path '.bar)))

  ; // get nested pair
  (match-params-fn 'match_1 (store fn)
    (define root-path (get store))
    (fn
      (get store '.baz)
      (get store '.boo '.first)))

  ; // get nested pair
  (match-params-fn 'match_2 (store fn)
    (fn (get store '.boo '.second)))

  ; // match variant alternative with overloaded fn.
  ; // equivalent to (get store 'my_var unit)
  (match-params-fn 'match_3 (store fn)
    (fn (get store '.my_var)))

  ; // match variant alternative with overloaded fn.
  ; // equivalent to (get store 'my_var unit)
  (match-params-fn 'match_4 (store fn)
    (match (get store '.my_var)
      (else => fn)))

  ; // match specific variant alternative std::string
  (match-params-fn 'match_5 (store fn)
    (define my_var
      (get store '.my_var))
    (match my_var
      ('std::string => fn)
      (else => noop)))

  ; // match variant index (not flat)
  (match-params-fn 'match_6 (store fn)
    (define my-var-index
      (get store '.my_var 'nbdl::variant_index))
    (fn my-var-index))

  ; // match variant index (not flat)
  (match-params-fn 'match_7 (store fn)
    (define my-var-index
      (get store '.my_var '|nbdl::variant_index_t{}|))
    (fn my-var-index))
}
}

TEST_CASE("Match context members", "[spec][match]") {
  std::string boo = "this is a boo";
  auto ctx = foo::context(5, std::move(boo), 9000);

  int result_foo = 0;
  std::string result_bar = "FAIL";
  int result_baz = 0;
  std::string result_boo_1;
  int result_boo_2 = 0;
  std::string result_alt_boo;
  size_t result_alt_index = ~size_t(-1);

  foo::match_0(ctx, [&](int foo, std::string const& bar) {
    result_foo = foo;
    result_bar = bar;
  });

  CHECK(result_foo == 42);
  CHECK(result_bar == std::string("initial string value for bar"));

  foo::match_1(ctx, [&](int baz, std::string const& boo_1) {
    result_baz = baz;
    result_boo_1 = boo_1;
  });

  CHECK(result_baz == 5);
  CHECK(result_boo_1 == std::string("this is a boo"));

  foo::match_2(ctx, [&](int& boo_2) {
    boo_2 += 1;
  });

  foo::match_2(ctx, [&](int const& boo_2) {
    result_boo_2 = boo_2;
  });

  CHECK(result_boo_2 == 9001);

  foo::match_3(ctx, boost::hana::overload_linearly(
    [&](std::string const& alt_boo) {
      result_alt_boo = alt_boo;
    },
    [&](auto&&) {
      result_alt_boo = "FAIL";
    }));

  CHECK(result_alt_boo == "this is a boo");

  result_alt_boo.clear();

  foo::match_4(ctx, boost::hana::overload_linearly(
    [&](std::string const& alt_boo) {
      result_alt_boo = alt_boo;
    },
    [&](auto&&) {
      result_alt_boo = "FAIL";
    }));

  CHECK(result_alt_boo == "this is a boo");

  result_alt_boo.clear();

  foo::match_5(ctx, [&](std::string const& alt_boo) {
    result_alt_boo = alt_boo;
  });

  CHECK(result_alt_boo == "this is a boo");

  foo::match_6(ctx, [&](int alt_index) {
    result_alt_index = alt_index;
  });

  CHECK(result_alt_index == 2);

  result_alt_index = ~size_t(0);
  foo::match_7(ctx, [&](int alt_index) {
    result_alt_index = alt_index;
  });

  CHECK(result_alt_index == 2);
}
