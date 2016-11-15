//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_TILES_GAME_HPP
#define NBDL_EXAMPLE_TILES_GAME_HPP

#include <nbdl.hpp>

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

  struct game_store { };

  template <typename Path>
  struct game_store_impl
  {
    using hana_tag = game_store;
    // just a singleton game right now
    game game_;

    // initialize the one game that we have :/
    game_store_impl()
      : game_({ 3, 3,
        {{
          3, 4, 6,
          8, 0, 1,
          2, 5, 7
        }}
      })
    { }

    // returns true if the game state actually changes
    template <typename GamePath, typename GameMove>
    bool apply_move(GamePath, GameMove gm)
    {
      return detail::apply_game_move(game_, gm);
    }
  };

  // need a better way to get path types
  using game_path = typename decltype(nbdl::path_type<int, tiles::game>)::type;
  using game_move_path = typename decltype(nbdl::path_type<int,
    tiles::game, tiles::game_move
  >)::type;

  template <typename StateConsumer, typename Index>
  void create_game_move(StateConsumer& c, Index i)
  {
    c.push_api.push(
      c.push_api.message_api()
        .make_upstream_create_message(game_move_path::make_create_path(), game_move{i})
    );
  }

  template <typename ProviderTag, typename ConsumerTag>
  struct context
  {
    static constexpr auto make_def()
    {
      using namespace nbdl_def; 
      namespace hana = boost::hana;
      return
        Context(
          Entities(
            Entity(Type(hana::type_c<game>)),
            Entity(Type(hana::type_c<game_move>))
          ),
          Provider(
            Type(ProviderTag{}),
            AccessPoint(
              Name(hana::type_c<void>), // FIXME
              EntityName(hana::type_c<game>),
              Store(hana::type_c<game_store>),
              Actions(),
              AccessPoint(
                Name(hana::type_c<void>), // FIXME
                EntityName(hana::type_c<game_move>),
                Actions(Create())
              )
            )
          ),
          Consumer(Type(ConsumerTag{}))
        );
      }
  };

  template <typename ProviderTag, typename ConsumerTag, typename ...Args>
  auto make_context(Args&& ... args)
  {
    return nbdl::make_unique_context<tiles::context<ProviderTag, ConsumerTag>>
      (std::forward<Args>(args)...);
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

  template <>
  struct make_store_impl<tiles::game_store>
  {
    template <typename PathType>
    static constexpr auto apply(PathType)
    {
      return tiles::game_store_impl<typename PathType::type>();
    }
  };

  template <>
  struct get_impl<tiles::game_store>
  {
    template <typename Store, typename Path>
    static constexpr auto apply(Store const& s, Path const&)
    {
      return s.game_;
    }
  };

  template <>
  struct apply_foreign_action_impl<tiles::game_store>
  {
    template <typename Store, typename Message, typename Fn>
    static constexpr auto apply(Store&& s, Message&& m, Fn&& fn)
    {
      if constexpr(decltype(hana::and_(
        hana::bool_c<message::is_create<Message>>,
        hana::bool_c<message::is_downstream<Message>>,
        hana::typeid_(message::get_path_type(m)) == hana::type_c<tiles::game_move_path>
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
