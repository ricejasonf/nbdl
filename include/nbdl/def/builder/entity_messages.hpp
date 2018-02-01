//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP
#define NBDL_DEF_BUILDER_ENTITY_MESSAGES_HPP

#include <mpdef/pair.hpp>
#include <mpdef/map.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/entity_message_is_from_root.hpp>
#include <nbdl/def/builder/entity_message_meta.hpp>
#include <nbdl/def/builder/entity_message_payload.hpp>
#include <nbdl/def/builder/entity_message_private_payload.hpp>
#include <nbdl/def/builder/entity_message_uid.hpp>
#include <nbdl/detail/make_create_path_type.hpp>
#include <nbdl/message.hpp>
#include <nbdl/tags.hpp>

#include <boost/hana.hpp>
#include <boost/mp11/list.hpp>
#include <utility>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;
namespace tag = nbdl_def::tag;
using namespace boost::mp11;

namespace entity_messages_detail {
  namespace action = nbdl::message::action;
  constexpr auto action_map = mpdef::make_map(
    mpdef::make_pair(tag::Create,      action::create{}),
    mpdef::make_pair(tag::Read,        action::read{}),
    mpdef::make_pair(tag::Update,      action::update{}),
    mpdef::make_pair(tag::Delete,      action::delete_{})
  );
  constexpr auto get_action = hana::partial(hana::at_key, action_map);
}

// make_upstream_messages

template <typename Action, typename ...>
struct make_upstream_messages;

template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
struct make_upstream_messages<nbdl::message::action::create
                            , Path
                            , Uid
                            , Payload
                            , PrivatePayload
                            >
{
  using Length = decltype(hana::length(std::declval<Path>()));
  static_assert(
    Length::value > 0
  , "Upstream message must have a path with at least one key."
  );
  using CreatePath = nbdl::detail::make_create_path_t<Path>;
  using type = mpdef::list<nbdl::message::upstream_create<CreatePath, Uid, Payload, PrivatePayload>>;
};

template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
struct make_upstream_messages<nbdl::message::action::read
                            , Path
                            , Uid
                            , Payload
                            , PrivatePayload
                            >
{
  using type = mpdef::list<nbdl::message::upstream_read<Path, Uid>>;
};

template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
struct make_upstream_messages<nbdl::message::action::update
                            , Path
                            , Uid
                            , Payload
                            , PrivatePayload
                            >
{
  using type = mpdef::list<nbdl::message::upstream_update<Path, Uid, Payload>>;
};

template <typename Path, typename Uid, typename Payload, typename PrivatePayload>
struct make_upstream_messages<nbdl::message::action::delete_
                            , Path
                            , Uid
                            , Payload
                            , PrivatePayload
                            >
{
  using type = mpdef::list<nbdl::message::upstream_delete<Path, Uid>>;
};

// make_downstream_messages

template <typename Action, typename ...>
struct make_downstream_messages;

template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
struct make_downstream_messages<nbdl::message::action::create
                            , Path
                            , Uid
                            , Payload
                            , IsConfirmed
                            >
{
  using type = mpdef::list<
    nbdl::message::downstream_create<Path, Uid, Payload, IsConfirmed>
  >;
};

template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
struct make_downstream_messages<nbdl::message::action::read
                            , Path
                            , Uid
                            , Payload
                            , IsConfirmed
                            >
{
  using type = mpdef::list<
    nbdl::message::downstream_read<Path, Uid, Payload>
  , nbdl::message::downstream_read<Path, Uid, nbdl::not_found>
  >;
};

template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
struct make_downstream_messages<nbdl::message::action::update
                            , Path
                            , Uid
                            , Payload
                            , IsConfirmed
                            >
{
  using type = mpdef::list<
    nbdl::message::downstream_update<Path, Uid, Payload, IsConfirmed>
  >;
};

template <typename Path, typename Uid, typename Payload, typename IsConfirmed>
struct make_downstream_messages<nbdl::message::action::delete_
                            , Path
                            , Uid
                            , Payload
                            , IsConfirmed
                            >
{
  using type = mpdef::list<
    nbdl::message::downstream_delete<Path, Uid, IsConfirmed>
  >;
};

// generate all messages for a given AccessPoint
struct entity_messages_fn
{
  template <typename AccessPoint>
  constexpr auto operator()(AccessPoint access_point) const
  {
    auto path = access_point_meta::path(access_point);
    auto entity_type = hana::back(
      access_point_meta::entities(access_point)
    );

    auto actions = hana::unpack(access_point_meta::actions(access_point),
      mpdef::make_list ^hana::on^ hana::compose(entity_messages_detail::get_action, hana::first));

    auto private_payload = nbdl::message::no_impl{};

    const auto messages = hana::unpack(
      actions
    , [=](auto ...action)
      {
        return mp_append<
          typename make_upstream_messages<
            decltype(action)
          , typename decltype(path)::type
          , nbdl::message::no_impl //nbdl::uid
          , typename decltype(entity_type)::type
          , decltype(private_payload)
          >::type...
        , typename make_downstream_messages<
            decltype(action)
          , typename decltype(path)::type
          , nbdl::message::no_impl //nbdl::uid
          , typename decltype(entity_type)::type
          , nbdl::message::no_impl //bool // is_confirmed
          >::type...
        >{};
      }
    );

    return messages;
  }
};

template <typename AccessPoint>
using entity_messages = decltype(entity_messages_fn{}(AccessPoint{}));

}//builder
}//nbdl_def
#endif
