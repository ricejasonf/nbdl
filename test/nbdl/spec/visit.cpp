//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/assign.hpp>
#include <nbdl/match.hpp>
#include <nbdl/strong_alias.hpp>
#include <nbdl/variant.hpp>

#include <catch.hpp>
#include <unordered_map>
#include <type_traits>

namespace my {

using fav_games = std::unordered_map<std::string, std::string>;

heavy_scheme {
  (import (heavy base)
          (nbdl spec))

  ; // Add to an integer sum.
  (define-store 'message_1 (id)
    (store-compose '.value (store 'int (init-args: id))))

  ; // Associate a player with a favorite game. (just strings)
  (define-store 'message_2 (name fav-game)
    (store-compose '.name (store 'std::string (init-args: name)))
    (store-compose '.fav_game (store 'std::string (init-args: fav-game))))

  ; // Remove a player from the favorite games table.
  (define-store 'message_3 (name)
    (store-compose '.name (store 'std::string (init-args: name))))

  ; // A tag.
  (define-store 'message_4 ())

  ; // A strong alias to a variant that implements nbdl::match.
  (define-store 'message ()
    (variant (store 'message_1)
             (store 'message_2)
             (store 'message_3)))

  (context 'message_context (arg)
    (member: '.body 'my::message (init-args: arg)))

  (context 'context ()
    (member: '.sum 'int (init-args:))
    (member: '.fav_games 'my::fav_games (init-args:))
    (member: '.message_4_count 'size_t (init-args:))
    (member: '.last_message 'message (init-args:))
    )

  (match-params-fn 'apply_message (context message fn)
    (define sum (get context '.sum))
    (define (receive-message-1 Message1)
      (visit 'nbdl::assign
             sum
             (visit '|std::plus<int>{}|
                    sum
                    (get Message1 '.value))))
    (define (receive-message-4 Message4)
      (match (get context '.message_4_count)
        (else => (lambda (count)
          (visit 'nbdl::assign
                 count ; // raw mlir.value as argument to visit
                 (visit '|std::plus<size_t>{}| count 1))))))
    (define (insert-fav-game msg)
      (visit '.insert
             (get context '.fav_games)
             (get msg '.name)
             (get msg '.fav_game)))
    (define (erase-fav-game msg)
      (visit '.erase
             (get context '.fav_games)
             (get msg '.name)))
    (match message
      ('my::message_1 => receive-message-1)
      ('my::message_2 => insert-fav-game)
      ('my::message_3 => erase-fav-game)
      ('my::message_4 => receive-message-4))
#| /* FIXME This creates the cartesian product of assignments
    *       on every pair of alternatives.
    *       Implement apply-action to assign directly to the variant object
    *       without visitation.
    (visit 'nbdl::assign
           (get context '.last_message)
           message)
    */
|#
    (fn (get context '.last_message)))
}
}

static_assert(std::is_assignable_v<my::message_1&, my::message_1>);
static_assert(std::is_assignable_v<my::message&, my::message_1>);
static_assert(std::is_assignable_v<my::message&, my::message_2>);
static_assert(std::is_assignable_v<my::message&, my::message_3>);
static_assert(std::is_assignable_v<my::message&, my::message_4>);
static_assert(nbdl::StoreAlias<my::message>);
static_assert(nbdl::Store<my::message>);

TEST_CASE("Apply messages", "[spec][visit]") {
  my::context context;
  my::message last_message;
  CHECK(context.sum == 0);
  my::message_1 message_1(3);
  my::apply_message(context, message_1, nbdl::noop);
  CHECK(context.sum == 3);
  my::apply_message(context, my::message_1(4), nbdl::noop);
  CHECK(context.sum == 7);
  my::apply_message(context, my::message_1(5), nbdl::noop);
  CHECK(context.sum == 12);

#if 0
  // FIXME apply-action to assign directly to variant object
  // FIXME Proxy relational operators in aliases.
  // CHECK(context.last_message == my::message_1(5));
  int LastMessageValue = 0;
  nbdl::match(context.last_message, [&](auto&& msg) {
      if constexpr(nbdl::SameAs<my::message_1, decltype(msg)>) {
        LastMessageValue = msg.value;
      }
    });
  CHECK(LastMessageValue == 5);  
#endif
  CHECK(context.message_4_count == 0);
  my::apply_message(context, my::message_4{}, nbdl::noop);
  CHECK(context.message_4_count == 1);
  my::apply_message(context, my::message_4{}, nbdl::noop);
  CHECK(context.message_4_count == 2);
  my::apply_message(context, my::message_4{}, nbdl::noop);
  CHECK(context.message_4_count == 3);
}
