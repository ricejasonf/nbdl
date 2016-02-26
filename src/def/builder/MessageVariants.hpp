//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_MAKE_MESSAGE_VARIANTS_HPP
#define NBDL_DEF_BUILDER_MAKE_MESSAGE_VARIANTS_HPP

#include<def/directives.hpp>
#include<def/builder/EntityMessages.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<Message.hpp>
#include<Variant.hpp>

#include<boost/hana.hpp>
#include<type_traits>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

// TODO possibly make this a metafunction
struct MakeMessageVariant
{
  template<typename EntityMessageTypes, typename SystemMessageType>
  constexpr auto operator()(EntityMessageTypes, SystemMessageType) const
  {
    // returns hana::type_c<nbdl::Variant<SystemMessages, EntityMessages...>>
    return hana::unpack(
      hana::prepend(EntityMessageTypes{}, SystemMessageType{}),
      hana::template_<nbdl::Variant>
    );
  }
};
constexpr MakeMessageVariant makeMessageVariant{};

// aggregate all entity messages
// and partition by upstream/downstream
struct MakeMessageVariants
{
  struct IsUpstreamHelper
  {
    template<typename Type>
    constexpr auto operator()(Type) const
    {
      using M = typename Type::type;
      return hana::bool_<decltype(
        hana::equal(
          hana::decltype_(nbdl::message::getChannel(std::declval<M>())),
          hana::type_c<nbdl::message::channel::Upstream>
        )
      )::value>{};
    }
  };

  struct Helper
  {
    template<typename UpstreamMessages, typename DownstreamMessages>
    constexpr auto operator()(UpstreamMessages, DownstreamMessages) const
    {
      // To this point we are working with two lists of types
      // TODO: see if using `hana::types` would be possible
      // return fn(system_message_type) -> Product of variants
      return hana::demux(hana::make_pair)
      (
        hana::partial(builder::makeMessageVariant, UpstreamMessages{}),
        hana::partial(builder::makeMessageVariant, DownstreamMessages{})
      );
    } 
  };

  template<typename EntityMap, typename ProvidersMeta>
  constexpr auto operator()(EntityMap entity_map, ProvidersMeta providers) const
  {
    // return a Metafunction that 
    // takes a SystemMessage type and returns
    // a Product of the upstream/downstream
    // message nbdl::Variants
    return hana::unpack(
      hana::partition(
        hana::unpack(
          hana::flatten(hana::unpack(providers, mpdef::make_list ^hana::on^ ProviderMeta::accessPoints)),
          hana::partial(builder::entityMessages, entity_map)
        ),
        IsUpstreamHelper{}
      ),
      Helper{}
    );
  }
};
constexpr MakeMessageVariants makeMessageVariants{};

}//builder
}//nbdl_def
#endif
