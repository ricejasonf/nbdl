//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/spec.hpp>
#include <catch.hpp>
#include <string>

  #include <iostream>

namespace foo {
heavy_scheme {
  (import (nbdl spec))

  (define (>= a b)
    (visit '.operator>= a b))

  (define-store 'weighted_string ()
    (store "std::pair<int, std::string>"))

  (define-store 'my_variant ()
    (variant (store 'nbdl::unresolved)
             (store 'weighted_string)))

  ; // Store 3 values and the result of concatenating
  ; // the strings that match our conditional.
  (define-store 'context (arg1 arg2 arg3)
    (store-compose '.val1 (store 'weighted_string
                            (init-args: arg1)))
    (store-compose '.val2 (store 'weighted_string
                            (init-args: arg2)))
    (store-compose '.val3 (store 'my_variant
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
      (define weight
        (get member '.first))
      (match-cond
        ((>= weight 42)
         (visit '.append (get context result-val)
                         member))
        (else (visit 'nbdl::noop))
        ))

    (visit '.clear result-val)
    (my-concat (get context '.val1))
    ;(my-concat (get context '.val2))
    ;(my-concat (get context '.val3))
    (fn 5))
  (dump-cpp 'weighted_string)
  (dump-cpp 'combo_concat)
  'ok
}
}  // namespace foo

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
  std::cout << ctx1.result_val << '\n';
  std::cout << ctx2.result_val << '\n';
  std::cout << ctx3.result_val << '\n';
  std::cout << ctx4.result_val << '\n';
  CHECK(ctx1.result_val == std::string());
  CHECK(ctx2.result_val == std::string("foobaz"));
  CHECK(ctx3.result_val == std::string("barbaz"));
  CHECK(ctx4.result_val == std::string("foobarbaz"));
}
