//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <catch.hpp>
#include <unordered_map>

namespace my {

using fav_games = std::unordered_map<std::string, std::string>;

heavy_scheme {
  (import (nbdl spec))

  // Add to an integer sum.
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
    (member: 'body 'my::message arg))

  (context 'context (this)
    (member: '.sum 'int 0)
    (member: 'fav_games 'my::fav_games)
    (member: '.message_4_count (store 'size_t))
    (member: '.last_message (store 'messages))  ;
    (store-compose '.message_4_count (store 'size_t))
    (store-compose '.last_message (store 'messages))
    )

  (define (plus Store1)
    (lambda (Store2)
      (apply-action Store1
        (visit 'std::plus
               Store1
               Store2))))

  (match-params-fn 'apply_message (context message fn)
    (define sum (get context '.sum))
    (define (insert-fav-game msg)
      (apply-action (get context '.fav_games)
                    '::nbdl::ext::std::unordered_map_insert
                    (get msg '.name)
                    (get msg '.fav_game)))
    (define (erase-fav-game msg)
      (apply-action (get context '.fav_games)
                    '::nbdl::ext::std::unordered_map_erase
                    (get msg '.name)))
    (match (get message)
      ('my::message_1 => (plus sum))
      ('my::message_2 => insert-fav-game)
      ('my::message_3 => erase-fav-game)
      ('my::message_4 => (plus 1)))
    (apply-action (get context '.last_message)
                  message))
}
}

TEST_CASE("Store interaction", "[spec][store-compose]") {
  // TODO
}
