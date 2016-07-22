//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_TILES_GAME_HPP
#define NBDL_EXAMPLE_TILES_GAME_HPP

#include <def/builder/Context.hpp>
#include <def/directives.hpp>
#include <nbdl/apply_foreign_action.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/message.hpp>
#include <Path.hpp>

#include <array>
#include <boost/hana/type.hpp>
#include <utility>

namespace tiles
{
  namespace detail
  {
    // Returns the index to swap with.
    template <typename GameState, typename Index>
    Index get_tile_swap_index(GameState const& g, Index i)
    {
      // Move the tile at `i` to the adjacent empty space if there is one.

      if (g.board[i] == 0)                  return i;               // current

      if (i % g.width != 0
        && g.board[i - 1] == 0)             return i - 1;           // left

      if (i % g.width != g.width - 1
        && g.board[i + 1] == 0)             return i + 1;           // right

      if (i >= g.width
        && g.board[i - g.width] == 0)       return i - g.width;     // above

      if (i <= g.height * g.width - g.width
        && g.board[i + g.width] == 0)       return i + g.width;     // below

      // The tile is not adjacent to the empty space.
      return i;
    }

    template <typename GameState, typename GameMove>
    bool apply_game_move(GameState&& game_, GameMove&& gm)
    {
      // Silently disregard out of range junk.
      auto i = std::forward<GameMove>(gm).index;
      if (i < 0 || i >= game_.width * game_.height)
      {
        return false;
      }
      auto i2 = detail::get_tile_swap_index(game_, i);
      if (i2 != i)
      {
        std::swap(game_.board[i], game_.board[i2]);
        return true;
      }
      return false;
    }
  }

  struct game
  {
    int width;
    int height;
    std::array<unsigned char, 64> board;
  };

  struct game_move
  {
    // The index of the tile to move to the
    // adjacent empty space.
    unsigned char index;
  };

  struct game_store
  {
    // just a singleton game right now
    game game_;

    // returns true if the game state actually changes
    template <typename GamePath, typename GameMove>
    bool apply_move(GamePath, GameMove gm)
    {
      return detail::apply_game_move(game_, gm);
    }
  };

  template <typename ProviderTag, typename ConsumerTag, typename ...Args>
  auto make_context(Args&& ... args)
  {
    using namespace nbdl_def; 
    namespace hana = boost::hana;
    return nbdl::make_unique_context(
      Context(
        Entities(
          Entity(Type(hana::type_c<game>)),
          Entity(Type(hana::type_c<game_move>))
        ),
        Provider(
          Type(hana::type_c<ProviderTag>),
          AccessPoint(
            Store(hana::type_c<game_store>),
            AccessPoint(
              Type(hana::type_c<game_move>),
              Actions(Create())
            )
          )
        ),
        Consumer(Type(hana::type_c<ConsumerTag>))
      ),
      std::forward<Args>(args)...
    );
  }
}

namespace nbdl
{
  NBDL_ENTITY(
    tiles::game
    , width
    , height
    , board
  );

  NBDL_ENTITY(
    tiles::game_move
    , index
  );

  // need a better way to get path types
  using game_move_path = typename decltype(nbdl::path_type<nbdl::no_key,
    tiles::game, tiles::game_move
  >)::type;

  template <>
  struct apply_foreign_action_impl<tiles::game_store>
  {
    template <typename Store, typename Message, typename Fn>
    static constexpr auto apply(Store&& s, Message&& m, Fn&& fn)
    {
      if constexpr(decltype(hana::and_(
        message::is_create<Message>,
        message::is_downstream<Message>,
        hana::typeid_(message::get_path_type(m)) == hana::type_c<game_move_path>
      ))::value)
      {
        auto parent_path = message::get_path(m).parent();
        if (std::forward<Store>(s).apply_move(
          parent_path,
          message::get_payload(std::forward<Message>(m)))
        )
        {
          std::forward<Fn>(fn)(std::move(parent_path));
        }
      }
    }
  };
}

#endif
