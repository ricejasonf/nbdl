//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_HPP
#define NBDL_BIND_HPP

#include<type_traits>
#include "EntityTraits.hpp"
#include "MemberSet.hpp"

namespace nbdl {

template<typename Binder, typename Entity>
void bind(Binder& binder, Entity& entity);

namespace details {

static auto bindEntity = [](auto& entity, auto& binder, auto member_type)
  -> std::enable_if_t<IsEntity<typename decltype(member_type)::type::MemberType>::value>
{
  using Member_ = typename decltype(member_type)::type;
  binder.bindEntity(MemberName<Member_>::value,
    [&entity](auto&& binder_x) {
      bind(binder_x, entity.*Member_::ptr);
    });
};

static auto bindMember = [](auto& entity, auto& binder, auto member_type)
{
  using Member_ = typename decltype(member_type)::type;
  binder.bindMember(MemberName<Member_>::value, entity.*Member_::ptr);
};

template<typename T, typename B>
static void callBindVariant_(T name, B binder) { binder.bindVariant(name, [](auto...){}); }

static auto bindVariant = [](auto& entity, auto& binder, auto member_type)
  -> typename decltype(member_type)::type::MemberType::VariantTag
{
  using Member_ = typename decltype(member_type)::type;
  auto name = MemberName<Member_>::value;
  auto variant = entity.*Member_::ptr; 
  binder.bindVariant(name, variant, hana::overload(
    [&](auto&& binder_x, auto type) {
      using T = typename decltype(type)::type;
      auto entity_ = T{};
      bind(binder_x, entity_);
      variant = entity;
    },
    [&](auto&& binder_x) {
      variant.match([&](auto entity_) {
        bind(binder_x, entity_);
      });
    }));
};

}//details

template<typename Binder, typename Entity>
void bind(Binder& binder, Entity& entity)
{
  hana::for_each(typename EntityTraits<Entity>::Members{},
    [&](auto member_type) {
      hana::overload_linearly(
        details::bindEntity,
        details::bindVariant,
        details::bindMember
      )(entity, binder, member_type);
    });
}

}//nbdl
#endif
