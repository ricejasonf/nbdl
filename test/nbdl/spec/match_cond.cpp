//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/spec.hpp>
#include <nbdl/ext/std/tuple_like.hpp>
#include <catch.hpp>
#include <functional>
#include <string>

namespace {
namespace foo {
heavy_scheme {
  (import (nbdl spec))

  (define (>= a b)
    (visit '|std::greater_equal<int>{}| a b))

  ; // Note that .first and .second are not accessible
  ; // because of implementation issues. (ie not having reflection)
  (define-store 'weighted_string ()
    (store "std::pair<int, std::string>"))

  ; // Store 3 values and the result of concatenating
  ; // the strings that match our conditional.
  (define-store 'context (arg1 arg2 arg3)
    (store-compose '.val1 (store 'weighted_string
                            (init-args: arg1)))
    (store-compose '.val2 (store 'weighted_string
                            (init-args: arg2)))
    (store-compose '.val3 (store 'weighted_string
                            (init-args: arg3)))
    (store-compose '.result_val (store 'std::string)))

  ; // Concatenate values with keys greater than 42.
  (match-params-fn 'combo_concat (context fn)
    ; // Concat a member (of context) to result_val
    ; // capitalizing it if its weight is greater than
    ; // the threshold 42.
    (define result-val
      (get context '.result_val))
    (define (my-concat member)
      ;; // This looks like destructuring here.
      (define weight
        (get member 'nbdl::index<0>))
      (define str
        (get member 'nbdl::index<1>))
      (match-cond
        ((>= weight 42)
         (visit '.append result-val str))
        (else (visit 'nbdl::noop))
        ))

    (visit '.clear result-val)
    (my-concat (get context '.val1))
    (my-concat (get context '.val2))
    (my-concat (get context '.val3))
    (fn 5))
}
}  // namespace foo
}  // namespace

TEST_CASE("Branch on conditionals over stores", "[spec][match-cond]") {
  foo::context ctx1(foo::weighted_string{0, "foo"},
                    foo::weighted_string{0, "bar"},
                    foo::weighted_string{0, "baz"});
  foo::context ctx2(foo::weighted_string{43, "foo"}, 
                    foo::weighted_string{5, "bar"}, 
                    foo::weighted_string{100, "baz"});
  foo::context ctx3(foo::weighted_string{0, "foo"}, 
                    foo::weighted_string{5, "bar"}, 
                    foo::weighted_string{100, "baz"});
  foo::context ctx4(foo::weighted_string{43, "foo"}, 
                    foo::weighted_string{9000, "bar"}, 
                    foo::weighted_string{100, "baz"});
  combo_concat(ctx1, nbdl::noop);
  combo_concat(ctx2, nbdl::noop);
  combo_concat(ctx3, nbdl::noop);
  combo_concat(ctx4, nbdl::noop);
  CHECK(ctx1.result_val == std::string());
  CHECK(ctx2.result_val == std::string("foobaz"));
  CHECK(ctx3.result_val == std::string("baz"));
  CHECK(ctx4.result_val == std::string("foobarbaz"));
}
