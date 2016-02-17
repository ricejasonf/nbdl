//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP

#include<def/directives.hpp>
#include<def/builder/EntityMessageIsFromRoot.hpp>
#include<def/builder/EntityMessageMeta.hpp>
#include<def/builder/EntityMessagePayload.hpp>
#include<def/builder/EntityMessagePrivatePayload.hpp>
#include<def/builder/Path.hpp>
#include<mpdef/Map.hpp>
#include<Message.hpp>

namespace nbdl_def {
namespace builder {

namespace tag = nbdl_def::tag;

namespace entity_messages_detail {
  constexpr auto action_map = mpdef::make_map(
    mpdef::make_tree_node(tag::Create,      nbdl::action::Create{}),
    mpdef::make_tree_node(tag::Read,        nbdl::action::Read{}),
    //mpdef::make_tree_node(tag::Update,      nbdl::action::Update{}),
    mpdef::make_tree_node(tag::UpdateRaw,   nbdl::action::UpdateRaw{}),
    mpdef::make_tree_node(tag::Delete,      nbdl::action::Delete{})
  );
  constexpr auto getAction = hana::partial(hana::at_key, action_map);
}

struct EntityMessage
{
  template<typename EntityMessageMeta>
  constexpr auto operator()(EntityMessageMeta e) const
  {
    return hana::type_c<
      hana::tuple<
        typename decltype(builder::entityMessageIsFromRoot(e))::type,
        decltype(e.channel()),
        decltype(e.action()),
        typename decltype(e.path())::type,
        typename decltype(builder::entityMessagePayload(e))::type,
        typename decltype(builder::entityMessagePrivatePayload(e))::type
      >
    >;
  }
};

// generate all messages for a given AccessPoint
struct EntityMessages
{
  template<typename EntityMap, typename AccessPoint>
  constexpr auto operator()(EntityMap entity_map, AccessPoint access_point) const
  {
    const auto path = builder::path(entity_map, access_point);
    const auto entity_type = hana::type_c<typename decltype(path)::type::Entity>;
    const auto private_payload = hana::transform(
      hana::find(access_point.actions(), tag::Create)
        | hana::reverse_partial(hana::find, tag::PrivatePayload)
        ,
      hana::second
    );

    const auto actions = hana::unpack(hana::second(access_point.actions()),
      mpdef::make_list ^hana::on^
        hana::compose(entity_messages_detail::getAction, hana::first)
    );

    const auto messages_meta = hana::unpack(
      hana::cartesian_product(mpdef::make_list(
        mpdef::make_list(path),
        mpdef::make_list(entity_type),
        mpdef::make_list(private_payload),
        actions,
        mpdef::make_list(nbdl::channel::Upstream{}, nbdl::channel::Downstream{})
      )),
      mpdef::make_list ^hana::on^ hana::fuse(builder::makeEntityMessageMeta)
    );

    // TODO: add ValidationFail response messages??

    return hana::unpack(messages_meta, mpdef::make_list ^hana::on^ EntityMessage{});
  }
};
constexpr EntityMessages entityMessages{};

}//builder
}//nbdl_def
#endif
