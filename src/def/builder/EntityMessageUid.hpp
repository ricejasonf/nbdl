//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_UID_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_UID_HPP

#include<def/directives.hpp>
#include<Message.hpp>
#include<Uid.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {
  // The message uid should be present
  // here unless the following are true:
  //
  //  - Message is requesting the creation
  //    of a locally versioned object.
  //    (the uid would be in the path)
  //
  //  - The action definition explicitly
  //    states that it doesn't care about
  //    receiving a response. (FireAndForget??)
  namespace tag = nbdl_def::tag;
  namespace action = nbdl::message::action;
  namespace channel = nbdl::message::channel;

  // upstream/create has uid in path
  template<typename AccessPoint>
  constexpr auto entityMessageUid(AccessPoint, action::Create, channel::Upstream)
  {
    // TODO - should be nothing if UseLocalVersion is true
    return hana::type_c<decltype(hana::just(std::declval<nbdl::Uid>()))>;
  }
  template<typename AccessPoint>
  constexpr auto entityMessageUid(AccessPoint /* access_point */, ...)
  {
    return hana::type_c<decltype(hana::just(std::declval<nbdl::Uid>()))>;
#if 0
    return hana::type_c<decltype(
      hana::if_(
        hana::find(access_point, tag::FireAndForget).value_or(hana::false_c),
        hana::just(std::declval<nbdl::Uid>()),
        hana::nothing
      )
    )>;
#endif
  }
}//detail

struct EntityMessageUid {
  template<typename A, typename M>
  constexpr auto operator()(A access_point, M entity_message_meta) const
  {
    return detail::entityMessageUid(
      access_point,
      entity_message_meta.action(),
      entity_message_meta.channel()
    );
  }
};
constexpr EntityMessageUid entityMessageUid{};

}//builder
}//nbdl_def
#endif
