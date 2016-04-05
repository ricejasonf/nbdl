//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGE_IS_FROM_ROOT_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGE_IS_FROM_ROOT_HPP

#include<def/directives.hpp>
#include<def/builder/EntityMessageMeta.hpp>
#include<nbdl/message.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

namespace detail {
  namespace tag = nbdl_def::tag;
  namespace channel = nbdl::message::channel;

  template<typename AccessPoint, typename Action>
  constexpr auto entity_message_is_from_root(AccessPoint/* access_point */, Action, channel::downstream)
  {
    return hana::type_c<hana::optional<bool>>;
#if 0
    // we only care if it is from root if we
    // have a local version of the object
    // UPDATE: Actually since we won't know
    // if the server might be sending downstream
    // messages that are confirmed by root then
    // all downstream messages should have this
    // bool.
    // FIXME
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
  constexpr auto entity_message_is_from_root(AccessPoint, Action, channel::upstream)
  { return hana::decltype_(hana::nothing); }
}//detail

struct entity_message_is_from_root_fn {
  template<typename A, typename M>
  constexpr auto operator()(A access_point, M em_meta) const
  {
    return detail::entity_message_is_from_root(
      access_point,
      entity_message_meta::action(em_meta),
      entity_message_meta::channel(em_meta)
    );
  }
};
constexpr entity_message_is_from_root_fn entity_message_is_from_root{};

}//builder
}//nbdl_def
#endif

