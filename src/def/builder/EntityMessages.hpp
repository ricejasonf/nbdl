//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP

#include <def/directives.hpp>
#include <def/builder/AccessPointMeta.hpp>
#include <def/builder/EntityMessageIsFromRoot.hpp>
#include <def/builder/EntityMessageMeta.hpp>
#include <def/builder/EntityMessagePayload.hpp>
#include <def/builder/EntityMessagePrivatePayload.hpp>
#include <def/builder/EntityMessageUid.hpp>
#include <def/builder/Path.hpp>
#include <mpdef/pair.hpp>
#include <mpdef/map.hpp>
#include <nbdl/make_store.hpp> // nbdl::not_found
#include <nbdl/message.hpp>

#include <boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;
namespace tag = nbdl_def::tag;

namespace entity_messages_detail {
  namespace action = nbdl::message::action;
  constexpr auto action_map = mpdef::make_map(
    mpdef::make_pair(tag::Create,      action::create{}),
    mpdef::make_pair(tag::Read,        action::read{}),
    mpdef::make_pair(tag::Update,      action::update{}),
    mpdef::make_pair(tag::UpdateRaw,   action::update_raw{}),
    mpdef::make_pair(tag::Delete,      action::delete_{})
  );
  constexpr auto get_action = hana::partial(hana::at_key, action_map);

  template <typename AccessPoint, typename EntityMessage, typename = void>
  struct get_maybes;

  template <typename A, typename E>
  struct get_maybes<A, E, std::enable_if_t<std::is_same<
        decltype(entity_message_meta::channel(E{})),
        nbdl::message::channel::upstream
      >::value>
  > 
  {
    // these are reversed to truncate the nothings
    using Xs = hana::tuple<
      typename decltype(builder::entity_message_private_payload(A{}, E{}))::type,
      typename decltype(builder::entity_message_payload(A{}, E{}))::type,
      typename decltype(builder::entity_message_uid(A{}, E{}))::type
    >;
    using type = decltype(hana::reverse(hana::drop_while(std::declval<Xs>(), hana::is_nothing)));
  };

  template <typename A, typename E>
  struct get_maybes<A, E, std::enable_if_t<std::is_same<
        decltype(entity_message_meta::channel(E{})),
        nbdl::message::channel::downstream
      >::value>
  > 
  {
    // these are reversed to truncate the nothings
    using Xs = hana::tuple<
      typename decltype(builder::entity_message_private_payload(A{}, E{}))::type,
      typename decltype(builder::entity_message_payload(A{}, E{}))::type,
      typename decltype(builder::entity_message_is_from_root(A{}, E{}))::type,
      typename decltype(builder::entity_message_uid(A{}, E{}))::type
    >;
    // these are reversed to truncate the nothings
    using type = decltype(hana::reverse(hana::drop_while(std::declval<Xs>(), hana::is_nothing)));
  };

  struct entity_message_meta_is_downstream_read_fn
  {
    template <typename EntityMessageMeta>
    constexpr auto operator()(EntityMessageMeta m)
    {
      return
            hana::decltype_(entity_message_meta::action(m))
              == hana::type_c<nbdl::message::action::read>
        &&  hana::decltype_(entity_message_meta::channel(m))
              == hana::type_c<nbdl::message::channel::downstream>
        ;
    }
  };
}

template <typename AccessPoint>
struct entity_message_fn
{
  template<typename EntityMessageMeta>
  constexpr auto operator()(EntityMessageMeta e) const
  {

    using Channel = decltype(entity_message_meta::channel(e));
    using Action = decltype(entity_message_meta::action(e));
    using Path_ = typename decltype(entity_message_meta::path(e))::type;
    using Path = typename decltype(hana::if_(
      hana::and_(
        std::is_same<Channel, nbdl::message::channel::upstream>{},
        std::is_same<Action, nbdl::message::action::create>{}
      ),
      Path_::make_create_path_type(), hana::type_c<Path_>
    ))::type;
    using Comps = 
      hana::tuple<
        Channel,
        Action,
        Path
      >;
    using Maybes = typename entity_messages_detail::get_maybes<AccessPoint, EntityMessageMeta>::type;
    using Tuple = decltype(hana::concat(std::declval<Comps>(), std::declval<Maybes>()));

    return hana::type_c<Tuple>;
  }
};

template <typename AccessPoint>
struct entity_not_found_message_fn
{
  template<typename EntityMessageMeta>
  constexpr auto operator()(EntityMessageMeta e) const
  {
    constexpr auto message_meta = builder::make_entity_message_meta(
      entity_message_meta::path(e),
      hana::decltype_(nbdl::not_found{}), // sub the entity with not_found
      entity_message_meta::private_payload(e),
      entity_message_meta::action(e),
      entity_message_meta::channel(e)
    );
    return entity_message_fn<AccessPoint>{}(message_meta);
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
      mpdef::make_list ^hana::on^ hana::compose(entity_messages_detail::get_action, hana::first));

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

    // add a nbdl::not_found message if a read is present
    return hana::concat(
      hana::unpack(messages_meta, mpdef::make_list ^hana::on^ entity_message_fn<AccessPoint>{}),
      hana::maybe(
        mpdef::make_list(),
        hana::compose(mpdef::make_list, entity_not_found_message_fn<AccessPoint>{}),
        hana::find_if(messages_meta,
          entity_messages_detail::entity_message_meta_is_downstream_read_fn{}))
    );
  }
};
constexpr entity_messages_fn entity_messages{};

}//builder
}//nbdl_def
#endif
