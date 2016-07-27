//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//

#include <nbdl/echo_provider.hpp>
#include <nbdl_test/test_state_consumer.hpp>
#include "../game.hpp"

#include <catch.hpp>

namespace
{
  auto context = tiles::make_context<nbdl::echo_provider, nbdl_test::state_consumer>();
  auto& state = context->cell<1>();
}

TEST_CASE(".", "[tiles][tiles_context]") 
{
  tiles::create_game_move(state, (unsigned char) 5);
  state.push_api.match(tiles::game_path{}, [](auto const& g)
  {
    auto expected = tiles::game{ 3, 3,
      {{
        3, 4, 6,
        8, 1, 0,
        2, 5, 7
      }}
    };
    CHECK(g.board == expected.board);
  });
}
