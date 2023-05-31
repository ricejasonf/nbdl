//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//

#include <boost/hana/string.hpp>
#include <nbdl.hpp>
#include <nbdl_test/test_state_consumer.hpp>
#include "../game.hpp"

#include <catch.hpp>

using namespace boost::hana::literals;

namespace
{
  struct context
  {
    static auto make_def()
    {
      return tiles::context<nbdl::echo_producer,
                            nbdl_test::state_consumer>::make_def();
    }
  };

  auto ctx = nbdl::make_context<context>(
    nbdl::actor("server"_s, nbdl::echo_producer{})
  );

  auto& state = ctx->template actor<1>();
}

TEST_CASE(".", "[tiles][tiles_context]") 
{
  tiles::create_game_move(state, (unsigned char) 5);
  nbdl::match(state.context, tiles::game_path{}, [](auto const& g)
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
