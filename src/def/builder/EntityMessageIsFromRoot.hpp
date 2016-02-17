//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_IS_FROM_ROOT_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_IS_FROM_ROOT_HPP

#include<def/directives.hpp>
#include<Message.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {
  namespace channel = nbdl::channel;

  template<typename EntityType, typename Action>
  constexpr auto entityMessageIsFromRoot(EntityType, Action, channel::Downstream)
  { return hana::decltype_(hana::just(bool{})); }

  template<typename EntityType, typename Action>
  constexpr auto entityMessageIsFromRoot(EntityType, Action, channel::Upstream)
  { return hana::decltype_(hana::nothing); }
}//detail

struct EntityMessageIsFromRoot {
  template<typename T>
  constexpr auto operator()(T entity_message_meta) const
  {
    return detail::entityMessageIsFromRoot(
      entity_message_meta.entityType(),
      entity_message_meta.action(),
      entity_message_meta.channel()
    );
  }
};
constexpr EntityMessageIsFromRoot entityMessageIsFromRoot{};

}//builder
}//nbdl_def
#endif

