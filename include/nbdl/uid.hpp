//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UID_HPP
#define NBDL_UID_HPP

#include <nbdl/entity.hpp>
#include <nbdl/key.hpp>

#include <array>
#include <boost/hana/ext/std/array.hpp>
#include <cstdint>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <random>
#endif

namespace nbdl
{
  using uid_storage = std::array<uint8_t, 16>;

  struct uid
  {
    uid() = delete;

    uid_storage value;
  };

  template <typename T>
  using unique_key = key<T, uid>;

  NBDL_ENTITY(uid, value);

  namespace detail
  {
#ifdef EMSCRIPTEN
    constexpr auto make_uid_generator = []
    {
      return []
      {
        uid_storage bytes{};
        for (int i = 0; i < 16; i++)
        {
          bytes[i] = (uint8_t) (emscripten_random() * 256);
        }
        return uid{std::move(bytes)};
      };
    };
#else
    constexpr auto make_uid_generator = []
    {
      static_assert(sizeof(uid_storage) == sizeof(std::array<uint32_t, 4>));
      std::random_device rd{};
      std::mt19937 mt_{rd()};

      return [mt = std::move(mt_)]() mutable
      {
        std::array<uint32_t, 4> temp{{
          (uint32_t) mt()
        , (uint32_t) mt()
        , (uint32_t) mt()
        , (uint32_t) mt()
        }};
        return uid{std::move(*reinterpret_cast<uid_storage*>(temp.data()))};
      };
    };
#endif
  }
}

#endif
