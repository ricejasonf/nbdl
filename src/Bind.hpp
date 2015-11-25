//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_HPP
#define NBDL_BIND_HPP

#include<utility>
#include<type_traits>
#include "Traits.hpp"

namespace nbdl {

namespace hana = boost::hana;

template<typename T1, typename T2, typename = void>
struct BindImpl
{
  template<typename Binder, typename X>
  static void apply(Binder&& binder, X&& x)
  {
    //use this?
    // std::forward<Binder>(binder).bindMember(std::forward<decltype(X)>(x));
    binder.bindMember(std::forward<X>(x));
  }
};

template<typename T1, typename T2>
struct BindImpl<T1, T2, EnableIfVariant<T2>>
{
  template<typename Binder, typename V>
  static void apply(Binder&& binder, V&& variant)
  {
    binder.bindVariant(std::forward<V>(variant));
  }
};

template<typename T1, typename T2>
struct BindImpl<T1, T2, EnableIfEntity<T2>>
{
  template<typename Binder, typename E>
  static void apply(Binder&& binder, E&& entity)
  {
    hana::unpack(
      hana::transform(entityMembers<E>(),
        [&](auto member_type) {
          return entityMember(entity, member_type);
        }),
      [&](auto&&... members) {
        binder.bindSequence(std::forward<decltype(members)>(members)...);
      });
  }
};

template<typename T1, typename T2>
struct BindImpl<T1, T2,
  std::enable_if_t<hana::Foldable<T2>::value>>
{
  template<typename Binder, typename Xs>
  static void apply(Binder&& binder, Xs&& xs)
  {
    hana::unpack(std::forward<Xs>(xs),
      [&](auto&&... members) {
        binder.bindSequence(std::forward<decltype(members)>(members)...);
      });
  }
};

template<typename Binder, typename Xs>
void bind(Binder&& binder, Xs&& xs)
{
  BindImpl<Binder, Xs>::apply(
    std::forward<Binder>(binder),
    std::forward<Xs>(xs));
}

}//nbdl
#endif
