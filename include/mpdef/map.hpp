//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_MAP_HPP
#define NBDL_MPDEF_MAP_HPP

#include <mpdef/list.hpp>

#include <boost/hana.hpp>
#include <type_traits>

namespace mpdef {

namespace hana = boost::hana;

namespace map_detail {
  template<typename Key, typename Value>
  struct elt { };

  template<typename Key, typename Value>
  Value get_(map_detail::elt<Key, Value>);

  template<typename Key, typename Value>
  hana::optional<Value> maybe_get_(map_detail::elt<Key, Value>);

  template<typename Key>
  hana::optional<> maybe_get_(...);

  template<typename Map, typename Key>
  using get = decltype(get_<std::add_const_t<Key>>(typename Map::type_table{}));

  template<typename Map, typename Key>
  using maybe_get = decltype(maybe_get_<std::add_const_t<Key>>(typename Map::type_table{}));
}

template<typename... Pairs>
struct map
{
  struct type_table
    : map_detail::elt<
        std::add_const_t<decltype(hana::first(Pairs{}))>,
        decltype(hana::second(Pairs{}))
      >...
  { };

  template<typename T>
  constexpr auto operator[](T) const
    -> map_detail::get<map<Pairs...>, T>
  { return {}; }
};

struct map_tag;

struct make_map_fn
{
  template<typename... Pairs>
  constexpr auto operator()(Pairs...) const
    -> mpdef::map<Pairs...>
  {
    return {};
  }
};
constexpr make_map_fn make_map{};

template<typename... PairsA, typename... PairsB>
static constexpr auto operator==(mpdef::map<PairsA...> a, mpdef::map<PairsB...> b)
{ return hana::equal(a, b); }

template<typename... PairsA, typename... PairsB>
static constexpr auto operator!=(mpdef::map<PairsA...> a, mpdef::map<PairsB...> b)
{ return hana::not_equal(a, b); }

}//mpdef

namespace boost { namespace hana {

  template<typename... Pairs>
  struct tag_of<mpdef::map<Pairs...>> { using type = mpdef::map_tag; };

  template<>
  struct keys_impl<mpdef::map_tag>
  {
    template<typename... Pairs>
    static constexpr auto apply(mpdef::map<Pairs...>)
      -> mpdef::list<decltype(hana::first(Pairs{}))...>
    {
      return {};
    }
  };

  // Comparable

  template<>
  struct equal_impl<mpdef::map_tag, mpdef::map_tag>
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
    static constexpr auto apply(mpdef::map<PairsA...> a, mpdef::map<PairsB...> b)
    {
      return helper(a, b, hana::bool_c<sizeof...(PairsA) == sizeof...(PairsB)>);
    }
  };

  // Foldable

  template<>
  struct unpack_impl<mpdef::map_tag>
  {
    template<typename... Pairs, typename F>
    static constexpr auto apply(mpdef::map<Pairs...>, F&& f)
    {
      return std::forward<F>(f)(Pairs{}...);
    }
  };

  // Searchable

  template<>
  struct at_key_impl<mpdef::map_tag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
      -> mpdef::map_detail::get<Map, Key>
    {
      return {};
    }
  };

  template<>
  struct find_impl<mpdef::map_tag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
      -> mpdef::map_detail::maybe_get<Map, Key>
    {
      return {};
    }
  };

  template<>
  struct contains_impl<mpdef::map_tag>
  {
    template<typename Map, typename Key>
    static constexpr auto apply(Map, Key)
    {
      return hana::is_just(mpdef::map_detail::maybe_get<Map, Key>{});
    }
  };

  template<>
  struct find_if_impl<mpdef::map_tag>
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
  struct any_of_impl<mpdef::map_tag>
  {
    template<typename Map, typename Pred>
    static constexpr auto apply(Map m, Pred&& pred)
    {
      return hana::is_just(hana::find_if(m, std::forward<Pred>(pred)));
    }
  };

}} // boost::hana
#endif
