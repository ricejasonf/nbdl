//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/assign.hpp>
#include <nbdl/strong_alias.hpp>
#include <nbdl/variant.hpp>

#include <catch.hpp>
#include <unordered_map>

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
    (member: 'fav_games 'my::fav_games (init-args:))
    (member: '.message_4_count 'size_t (init-args:))
    (member: '.last_message 'message (init-args:))
    )

  (define (plus Store1)
    (lambda (Store2)
      (visit 'nbdl::assign
             Store1
             (visit 'std::plus
                    Store1
                    Store2))))

  (match-params-fn 'apply_message (context message fn)
    (define sum (get context '.sum))
    (define (insert-fav-game msg)
      (visit '.insert
             (get context '.fav_games)
             (get msg '.name)
             (get msg '.fav_game)))
    (define (erase-fav-game msg)
      (visit '.erase
             (get context '.fav_games)
             (get msg '.name)))
    (match (get message)
           ('my::message_1 => (plus sum))
           ('my::message_2 => insert-fav-game)
           ('my::message_3 => erase-fav-game)
           ('my::message_4 => (plus 1)))
    (visit 'nbdl::assign
           (get context '.last_message)
           message)
    (fn (get context '.last_message)))
}
}

TEST_CASE("Store interaction", "[spec][store-compose]") {
  // TODO
}
