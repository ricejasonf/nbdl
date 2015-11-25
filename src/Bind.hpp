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

/*
template<typename Binder, typename Entity>
void bind(Binder& binder, Entity& entity);

namespace details {

static auto bindEntity = [](auto& entity, auto& binder, auto member_type)
  -> std::enable_if_t<IsEntity<typename decltype(member_type)::type::MemberType>::value>
{
  using Member_ = typename decltype(member_type)::type;
  binder.bindEntity(MemberName<Member_>::value,
    [&entity](auto& binder_x) {
      bind(binder_x, entity.*Member_::ptr);
    });
};

static auto bindMember = [](auto& entity, auto& binder, auto member_type)
{
  using Member_ = typename decltype(member_type)::type;
  binder.bindMember(MemberName<Member_>::value, entity.*Member_::ptr);
};

static auto bindVariant = [](auto& entity, auto& binder, auto member_type)
  -> typename decltype(member_type)::type::MemberType::VariantTag
{
  using Member_ = typename decltype(member_type)::type;
  auto& variant = entity.*Member_::ptr; 
  binder.bindVariant(MemberName<Member_>::value, variant, hana::overload_linearly(
    [&](auto& binder_x, auto entity_)
      -> EnableIfEntity<decltype(entity_)>
    {
      bind(binder_x, entity_);
    },
    [&](auto& binder_x, auto type)
    {
      using T = typename decltype(type)::type;
      auto entity_ = T{};
      bind(binder_x, entity_);
      variant = entity_;
    }));
};

}//details
*/

template<typename T1, typename T2, typename = void>
struct BindImpl
{
  template<typename Binder, typename Xs>
  void apply(Binder&& binder, Xs&& xs)
  {
    binder.bindMember(std::forward<decltype(auto)>(members)...);
  }
};

template<typename T1, typename E_>
struct BindImpl<T1, E_, EnableIfEntity<E_>>
{
  template<typename Binder, typename E>
  void apply(Binder&& binder, E&& entity)
  {
    hana::unpack(
      hana::transform(typename EntityTraits<E_>::Members{},
        [&](auto member_type) {
          using Member_ = typename decltype(member_type)::type;
          return entity.*Member_::ptr;
        }),
      [&](auto&&... members) {
        binder.bindSequence(std::forward<decltype(auto)>(members)...);
      });
  }
};

template<typename T1, typename V_>
struct BindImpl<T1, V, EnableIfVariant<V_>>
{
  template<typename Binder, typename V>
  void apply(Binder&& binder, V&& variant)
  {
    hana::unpack(std::forward<Xs>(xs),
      [&](auto&&... members) {
        binder.bindSequence(std::forward<decltype(auto)>(members)...);
      });
  }
  template<typename Binder, typename Xs>
  void apply(Binder&& binder, const Xs& xs)
  {
    binder.bindSequqnce(variant.getTypeId(),
      variant.match(
        [](auto value) {
          
        }));
  }
};

template<typename T1, typename T2>
struct BindImpl<T1, T2,
  std::enable_if_t<hana::Foldable<T2>::value>>
{
  template<typename Binder, typename Xs>
  void apply(Binder&& binder, Xs&& xs)
  {
    hana::unpack(std::forward<Xs>(xs),
      [&](auto&&... members) {
        binder.bindSequence(std::forward<decltype(auto)>(members)...);
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
