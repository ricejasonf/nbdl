//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_HPP
#define NBDL_BIND_HPP

#include<Traits.hpp>

#include<boost/hana.hpp>
#include<type_traits>
#include<utility>

namespace nbdl {

namespace hana = boost::hana;

template<typename T1, typename T2, typename = void>
struct bind_impl
{
  template<typename Binder, typename X>
  static void apply(Binder&& binder, X&& x)
  {
    //use this?
    // std::forward<Binder>(binder).bind_member(std::forward<X>(x));
    binder.bind_member(std::forward<X>(x));
  }
};

template<typename T1, typename T2>
struct bind_impl<T1, T2, enable_if_variant<T2>>
{
  template<typename Binder, typename V>
  static void apply(Binder&& binder, V&& variant)
  {
    binder.bind_variant(std::forward<V>(variant));
  }
};

template<typename T1, typename T2>
struct bind_impl<T1, T2, enable_if_entity<T2>>
{
  template<typename Binder, typename E>
  static void apply(Binder&& binder, E&& entity)
  {
    hana::unpack(
      hana::transform(entity_members<E>(),
        [&](auto member_type) {
          return entity_member(entity, member_type);
        }),
      [&](auto&&... members) {
        binder.bind_sequence(std::forward<decltype(members)>(members)...);
      });
  }
};

template<typename T1, typename T2>
struct bind_impl<T1, T2,
  std::enable_if_t<hana::Foldable<T2>::value>>
{
  template<typename Binder, typename Xs>
  static void apply(Binder&& binder, Xs&& xs)
  {
    hana::unpack(std::forward<Xs>(xs),
      [&](auto&&... members) {
        binder.bind_sequence(std::forward<decltype(members)>(members)...);
      });
  }
};

template<typename Binder, typename Xs>
void bind(Binder&& binder, Xs&& xs)
{
  bind_impl<typename std::decay<Binder>::type, typename std::decay<Xs>::type>::apply(
    std::forward<Binder>(binder),
    std::forward<Xs>(xs));
}

}//nbdl
#endif
