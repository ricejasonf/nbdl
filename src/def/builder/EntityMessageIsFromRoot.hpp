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
  namespace tag = nbdl_def::tag;
  namespace channel = nbdl::message::channel;

  template<typename AccessPoint, typename Action>
  constexpr auto entityMessageIsFromRoot(AccessPoint/* access_point */, Action, channel::Downstream)
  {
    return hana::decltype_(hana::nothing);
#if 0
    // we only care if it is from root if we
    // have a local version of the object
    return hana::type_c<decltype(
      hana::if_(
        hana::find(access_point, tag::UseLocalVersion).value_or(hana::false_c),
        hana::just(bool{}),
        hana::nothing
      )
    )>;
#endif
  }

  template<typename AccessPoint, typename Action>
  constexpr auto entityMessageIsFromRoot(AccessPoint, Action, channel::Upstream)
  { return hana::decltype_(hana::nothing); }
}//detail

struct EntityMessageIsFromRoot {
  template<typename A, typename M>
  constexpr auto operator()(A access_point, M entity_message_meta) const
  {
    return detail::entityMessageIsFromRoot(
      access_point,
      entity_message_meta.action(),
      entity_message_meta.channel()
    );
  }
};
constexpr EntityMessageIsFromRoot entityMessageIsFromRoot{};

}//builder
}//nbdl_def
#endif

