//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_MAP_HPP
#define NBDL_MPDEF_MAP_HPP

#include<mpdef/List.hpp>

#include<boost/hana.hpp>
#include<type_traits>

namespace mpdef {

namespace hana = boost::hana;

namespace map_detail {
  template<typename Key, typename Value>
  struct elt { };

  template<typename Key, typename Value>
  Value get(map_detail::elt<Key, Value>);

  template<typename Key, typename Value>
  hana::optional<Value> maybe_get(map_detail::elt<Key, Value>);

  template<typename Key>
  hana::optional<> maybe_get(...);

  template<typename Map, typename Key>
  using Get = decltype(get<std::add_const_t<Key>>(typename Map::TypeTable{}));

  template<typename Map, typename Key>
  using MaybeGet = decltype(maybe_get<std::add_const_t<Key>>(typename Map::TypeTable{}));
}

template<typename... Pairs>
struct Map
{
  struct TypeTable
    : map_detail::elt<
        std::add_const_t<decltype(hana::first(Pairs{}))>,
        decltype(hana::second(Pairs{}))
      >...
  { };

  template<typename T>
  constexpr auto operator[](T) const
    -> map_detail::Get<Map<Pairs...>, T>
  { return {}; }
};

struct MapTag;

struct MakeMap
{
  template<typename... Pairs>
  constexpr auto operator()(Pairs...) const
    -> mpdef::Map<Pairs...>
  {
    return {};
  }
};
constexpr MakeMap make_map{};

template<typename... PairsA, typename... PairsB>
static constexpr auto operator==(mpdef::Map<PairsA...> a, mpdef::Map<PairsB...> b)
{ return hana::equal(a, b); }

template<typename... PairsA, typename... PairsB>
static constexpr auto operator!=(mpdef::Map<PairsA...> a, mpdef::Map<PairsB...> b)
{ return hana::not_equal(a, b); }

}//mpdef

namespace boost { namespace hana {

  template<typename... Pairs>
  struct tag_of<mpdef::Map<Pairs...>> { using type = mpdef::MapTag; };

  template<>
  struct keys_impl<mpdef::MapTag>
  {
    template<typename... Pairs>
    static constexpr auto apply(mpdef::Map<Pairs...>)
      -> mpdef::List<decltype(hana::first(Pairs{}))...>
    {
      return {};
    }
  };

  // Comparable

  template<>
  struct equal_impl<mpdef::MapTag, mpdef::MapTag>
  {
    template<typename A, typename B>
    static constexpr auto helper(A, B, hana::false_)
    {
      return hana::false_c;
    }

    template<typename A, typename B>
    static constexpr auto helper(A a, B b, hana::true_)
    {
      return hana::all_of(hana::keys(a),
        hana::demux(hana::equal)
        (
          hana::partial(hana::find, a),
          hana::partial(hana::find, b)
        )
      );
    }

    template<typename... PairsA, typename... PairsB>
    static constexpr auto apply(mpdef::Map<PairsA...> a, mpdef::Map<PairsB...> b)
    {
      return helper(a, b, hana::bool_c<sizeof...(PairsA) == sizeof...(PairsB)>);
    }
  };

  // Foldable

  template<>
  struct unpack_impl<mpdef::MapTag>
  {
    template<typename... Pairs, typename F>
    static constexpr auto apply(mpdef::Map<Pairs...>, F&& f)
    {
      return std::forward<F>(f)(Pairs{}...);
    }
  };

  // Searchable

  template<>
  struct at_key_impl<mpdef::MapTag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
      -> mpdef::map_detail::Get<Map, Key>
    {
      return {};
    }
  };

  template<>
  struct find_impl<mpdef::MapTag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
      -> mpdef::map_detail::MaybeGet<Map, Key>
    {
      return {};
    }
  };

  template<>
  struct contains_impl<mpdef::MapTag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
    {
      return hana::is_just(mpdef::map_detail::MaybeGet<Map, Key>{});
    }
  };

  template<>
  struct find_if_impl<mpdef::MapTag>
  {
    template<typename Map, typename Pred>
    static constexpr auto apply(Map m, Pred&& pred)
    {
      // not exactly optimized
      return hana::find_if(
        hana::unpack(m, hana::make_tuple ^hana::on^ hana::first),
        std::forward<Pred>(pred)
      ) 
        | hana::compose(hana::just, hana::partial(hana::at_key, m))
        ;
    }
  };

  template<>
  struct any_of_impl<mpdef::MapTag>
  {
    template<typename Map, typename Pred>
    static constexpr auto apply(Map m, Pred&& pred)
    {
      return hana::is_just(hana::find_if(m, std::forward<Pred>(pred)));
    }
  };

}} // boost::hana
#endif
