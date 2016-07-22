//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//

#include "../game.hpp"

#include <catch.hpp>

TEST_CASE("Move tile to left.", "[tiles][game_apply_move]") 
{
  tiles::game x{
    3, 3,
    {{
      1, 1, 1,
      1, 0, 2,
      1, 1, 1
    }}
  }; 

  tiles::game expected{
    3, 3,
    {{
      1, 1, 1,
      1, 2, 0,
      1, 1, 1
    }}
  }; 
  CHECK(tiles::detail::apply_game_move(x, tiles::game_move{5}));
  CHECK(x.board == expected.board);
}

TEST_CASE("Move tile to right.", "[tiles][game_apply_move]") 
{
  tiles::game x{
    3, 3,
    {{
      1, 1, 1,
      1, 2, 0,
      1, 1, 1
    }}
  }; 

  tiles::game expected{
    3, 3,
    {{
      1, 1, 1,
      1, 0, 2,
      1, 1, 1
    }}
  }; 
  CHECK(tiles::detail::apply_game_move(x, tiles::game_move{4}));
  CHECK(x.board == expected.board);
}

TEST_CASE("Move tile up.", "[tiles][game_apply_move]") 
{
  tiles::game x{
    3, 3,
    {{
      1, 1, 1,
      1, 1, 0,
      1, 1, 2
    }}
  }; 

  tiles::game expected{
    3, 3,
    {{
      1, 1, 1,
      1, 1, 2,
      1, 1, 0
    }}
  }; 
  CHECK(tiles::detail::apply_game_move(x, tiles::game_move{8}));
  CHECK(x.board == expected.board);
}

TEST_CASE("Move tile down.", "[tiles][game_apply_move]") 
{
  tiles::game x{
    3, 3,
    {{
      2, 1, 1,
      0, 1, 1,
      1, 1, 1
    }}
  }; 

  tiles::game expected{
    3, 3,
    {{
      0, 1, 1,
      2, 1, 1,
      1, 1, 1
    }}
  }; 
  CHECK(tiles::detail::apply_game_move(x, tiles::game_move{0}));
  CHECK(x.board == expected.board);
}

TEST_CASE("An invalid move should not change state.", "[tiles][game_apply_move]") 
{
  {
    tiles::game x{
      3, 3,
      {{
        0, 1, 1,
        1, 1, 1,
        1, 1, 1
      }}
    }; 

    tiles::game expected = x;
    CHECK(false == tiles::detail::apply_game_move(x, tiles::game_move{0}));
    CHECK(x.board == expected.board);
  }
  {
    tiles::game x{
      3, 3,
      {{
        1, 1, 1,
        0, 1, 1,
        1, 1, 1
      }}
    }; 

    tiles::game expected = x;
    CHECK(false == tiles::detail::apply_game_move(x, tiles::game_move{2}));
    CHECK(x.board == expected.board);
  }
}
