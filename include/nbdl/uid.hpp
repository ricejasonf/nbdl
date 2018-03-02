//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UID_HPP
#define NBDL_UID_HPP

#include <nbdl/concept/Buffer.hpp>
#include <nbdl/detail/hash_combine.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/key.hpp>

#include <array>
#include <boost/hana/ext/std/array.hpp>
#include <cstdint>
#include <iterator>
#include <functional>
#include <tuple>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <random>
#endif

namespace nbdl
{
  struct make_uid_generator_fn
  {
    auto operator()() const;
  };

  constexpr make_uid_generator_fn make_uid_generator{};

  struct uid
  {
    using value_type = uint8_t;

    uid() = default;

    constexpr auto size()         { return storage.size(); };
    constexpr auto size() const   { return storage.size(); };
    constexpr auto data()         { return storage.data(); };
    constexpr auto data() const   { return storage.data(); };
    constexpr auto begin()        { return storage.begin(); };
    constexpr auto end()          { return storage.end(); };
    constexpr auto begin() const  { return storage.begin(); };
    constexpr auto end()   const  { return storage.end(); };

    inline decltype(auto) operator[](std::size_t i) const { return storage[i]; };
    inline decltype(auto) operator[](std::size_t i)       { return storage[i]; };

    inline bool operator==(nbdl::uid const& o) const
    { return storage == o.storage; }
    inline bool operator!=(nbdl::uid const& o) const
    { return storage != o.storage; }

  private:
    using storage_t = std::array<value_type, 16>;
    friend make_uid_generator_fn;
    friend void swap(nbdl::uid&, nbdl::uid&);

    uid(storage_t&& s)
      : storage(std::move(s))
    { }

    storage_t storage;
  };

  // swap

  inline void swap(nbdl::uid& x, nbdl::uid& y)
  {
    std::swap(x.storage, y.storage);
  }

  template <typename T>
  using unique_key = key<T, uid>;

#ifdef EMSCRIPTEN
  inline auto make_uid_generator_fn::operator()() const
  {
    return []
    {
      nbdl::uid xs{};
      for (auto& x : xs)
      {
        x = (uint8_t) (emscripten_random() * 256);
      }
      return xs;
    };
  };
#else
  inline auto make_uid_generator_fn::operator()() const
  {
    static_assert(sizeof(typename uid::storage_t) == sizeof(std::array<uint32_t, 4>));
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
      return uid{std::move(*reinterpret_cast<typename uid::storage_t*>(temp.data()))};
    };
  };
#endif
}

namespace std
{
  // hash

  template <>
  class hash<nbdl::uid>
  {
  public:
    std::size_t operator()(nbdl::uid const& uid) const 
    {
      std::size_t digest = 0;
      for (auto const& x : uid)
      {
        nbdl::detail::hash_combine(digest, x);
      }
      return digest;
    }
  };

  // tuple_size
  
  template <>
  class tuple_size<nbdl::uid>
    : public std::integral_constant<std::size_t, 16>
  { };

}

#endif
