//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_METASTRUCT_HPP
#define NBDL_MPDEF_METASTRUCT_HPP

#include <mpdef/map.hpp>
#include <mpdef/tree_node.hpp>

#include <boost/hana.hpp>
#include <boost/mp11/bind.hpp>

namespace mpdef {

namespace hana = boost::hana;

struct metastruct_tag { };

template<typename Impl>
struct metastruct
{
  template<typename ...T>
  struct list { using hana_tag = mpdef::metastruct_tag ; };

  template<std::size_t i>
  struct at_fn
  {
    static constexpr std::size_t value = i;

    template<typename... T>
    constexpr auto operator()(list<T...>) const
      -> decltype(hana::arg<i + 1>(T{}...))
    { return {}; }

    template<typename T>
    constexpr auto operator=(T) const
      -> mpdef::tree_node<at_fn<i>, T>
    { return {}; }
  };

  template<std::size_t i>
  static constexpr at_fn<i> at_c{};
};

template<typename  M>
struct make_metastruct_fn
{
  template<typename... T>
  constexpr auto operator()(T...) const
  {
    return typename M::template list<T...>{};
  }
};

template<typename M>
constexpr make_metastruct_fn<M> make_metastruct{};

template<typename M>
struct make_metastruct_with_map_fn
{
  template<typename... Pairs>
  constexpr auto operator()(Pairs ...pairs) const
  {
    static_assert(sizeof...(Pairs) == decltype(hana::length(M::spec))::value
      , "Keys provided must match the spec for the Metastruct.");
    return hana::unpack(M::spec,
      hana::on(
        make_metastruct<M>,
        hana::partial(hana::at_key, mpdef::make_map(pairs...))
      )
    );
  }
};

template<typename M>
constexpr make_metastruct_with_map_fn<M> make_metastruct_with_map{};

template <typename Key, typename List>
using metastruct_get_impl = decltype(Key{}(List{}));

template <typename Key>
using metastruct_get = boost::mp11::mp_bind_front<metastruct_get_impl, Key>;

} // mpdef

namespace boost { namespace hana {
  template<>
  struct equal_impl<mpdef::metastruct_tag, mpdef::metastruct_tag>
  {
    template<typename T, typename U>
    static constexpr auto apply(T, U)
      -> hana::false_
    { return {}; }

    template<typename T>
    static constexpr auto apply(T, T)
      -> hana::true_
    { return {}; }
  };

}} // boost::hana

#endif
