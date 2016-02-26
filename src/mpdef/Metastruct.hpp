//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_METASTRUCT_HPP
#define NBDL_MPDEF_METASTRUCT_HPP

#include<mpdef/Map.hpp>
#include<mpdef/TreeNode.hpp>

#include<boost/hana.hpp>

namespace mpdef {

namespace hana = boost::hana;

struct MetastructTag { };

template<typename Impl>
struct Metastruct
{
  template<typename ...T>
  struct List { using hana_tag = mpdef::MetastructTag ; };

  template<std::size_t i>
  struct At
  {
    template<typename... T>
    constexpr auto operator()(List<T...>) const
      -> decltype(hana::arg<i + 1>(T{}...))
    { return {}; }

    template<typename T>
    constexpr auto operator=(T) const
      -> mpdef::TreeNode<At<i>, T>
    { return {}; }
  };

  template<std::size_t i>
  static constexpr At<i> at_c{};
};

template<typename  M>
struct MakeMetastruct
{
  template<typename... T>
  constexpr auto operator()(T...) const
  {
    return typename M::template List<T...>{};
  }
};

template<typename M>
constexpr MakeMetastruct<M> makeMetastruct{};

template<typename M>
struct MakeMetastructWithMap
{
  template<typename... Pairs>
  constexpr auto operator()(Pairs ...pairs) const
  {
    static_assert(sizeof...(Pairs) == decltype(hana::length(M::spec))::value
      , "Keys provided must match the spec for the Metastruct.");
    return hana::unpack(M::spec,
      hana::on(
        MakeMetastruct<M>{},
        hana::partial(hana::at_key, mpdef::make_map(pairs...))
      )
    );
  }
};

template<typename M>
constexpr MakeMetastructWithMap<M> makeMetastructWithMap{};


} // mpdef

namespace boost { namespace hana {
  template<>
  struct equal_impl<mpdef::MetastructTag, mpdef::MetastructTag>
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
