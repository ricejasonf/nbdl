//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP

#include<def/directives.hpp>
#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMessageIsFromRoot.hpp>
#include<def/builder/EntityMessageMeta.hpp>
#include<def/builder/EntityMessagePayload.hpp>
#include<def/builder/EntityMessagePrivatePayload.hpp>
#include<def/builder/EntityMessageUid.hpp>
#include<def/builder/Path.hpp>
#include<mpdef/Map.hpp>
#include<nbdl/message.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;
namespace tag = nbdl_def::tag;

namespace entity_messages_detail {
  namespace action = nbdl::message::action;
  constexpr auto action_map = mpdef::make_map(
    mpdef::make_tree_node(tag::Create,      action::create{}),
    mpdef::make_tree_node(tag::Read,        action::read{}),
    mpdef::make_tree_node(tag::Update,      action::update{}),
    mpdef::make_tree_node(tag::UpdateRaw,   action::update_raw{}),
    mpdef::make_tree_node(tag::Delete,      action::delete_{})
  );
  constexpr auto get_action = hana::partial(hana::at_key, action_map);
}

// TODO possibly make this a metafunction
template <typename AccessPoint>
struct entity_message_fn
{
  template<typename EntityMessageMeta>
  constexpr auto operator()(EntityMessageMeta e) const
  {
    constexpr AccessPoint a{};
    return hana::type_c<
      hana::tuple<
        decltype(entity_message_meta::channel(e)),
        decltype(entity_message_meta::action(e)),
        typename decltype(entity_message_meta::path(e))::type,
        typename decltype(builder::entity_message_is_from_root(a, e))::type,
        typename decltype(builder::entity_message_uid(a, e))::type,
        typename decltype(builder::entity_message_payload(a, e))::type,
        typename decltype(builder::entity_message_private_payload(a, e))::type
      >
    >;
  }
};

// generate all messages for a given AccessPoint
struct entity_messages_fn
{
  template<typename EntityMap, typename AccessPoint>
  constexpr auto operator()(EntityMap entity_map, AccessPoint access_point) const
  {
    const auto path = builder::path(entity_map, access_point);
    const auto entity_type = hana::type_c<typename decltype(path)::type::Entity>;
  #if 0
    // TODO move definition of PrivatePayload into AccessPoint
    // and add that to AccessPointMeta
    // actions can be leaf nodes
    const auto private_payload = hana::transform(
      hana::find(AccessPointMeta::actions(access_point), tag::Create)
        | hana::reverse_partial(hana::find, tag::PrivatePayload)
        ,
      hana::second
    );
    const auto actions = hana::unpack(hana::second(AccessPointMeta::actions(access_point)),
      mpdef::make_list ^hana::on^
        hana::compose(entity_messages_detail::getAction, hana::first)
    );
  #endif
    const auto actions = hana::unpack(access_point_meta::actions(access_point),
      mpdef::make_list ^hana::on^ entity_messages_detail::get_action);

    const auto private_payload = hana::nothing;

    const auto messages_meta = hana::unpack(
      hana::cartesian_product(mpdef::make_list(
        mpdef::make_list(path),
        mpdef::make_list(entity_type),
        mpdef::make_list(private_payload),
        actions,
        mpdef::make_list(nbdl::message::channel::upstream{}, nbdl::message::channel::downstream{})
      )),
      mpdef::make_list ^hana::on^ hana::fuse(builder::make_entity_message_meta)
    );

    // TODO: add ValidationFail response messages??

    return hana::unpack(messages_meta,
      mpdef::make_list ^hana::on^ entity_message_fn<AccessPoint>{}
    );
  }
};
constexpr entity_messages_fn entity_messages{};

}//builder
}//nbdl_def
#endif
