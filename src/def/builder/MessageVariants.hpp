//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_MAKE_MESSAGE_VARIANTS_HPP
#define NBDL_DEF_BUILDER_MAKE_MESSAGE_VARIANTS_HPP

#include <def/directives.hpp>
#include <def/builder/EntityMessages.hpp>
#include <def/builder/ProviderMeta.hpp>
#include <nbdl/message.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana.hpp>
#include <type_traits>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

// TODO possibly make this a metafunction
struct make_message_variant_fn
{
  template<typename EntityMessageTypes, typename SystemMessageType>
  constexpr auto operator()(EntityMessageTypes, SystemMessageType) const
  {
    // returns hana::type_c<nbdl::variant<SystemMessages, EntityMessages...>>
    return hana::unpack(
      hana::prepend(EntityMessageTypes{}, SystemMessageType{}),
      hana::template_<nbdl::variant>
    );
  }
};
constexpr make_message_variant_fn make_message_variant{};

// aggregate all entity messages
// and partition by upstream/downstream
struct make_message_variants_fn
{
  struct is_upstream_helper_fn
  {
    template<typename Type>
    constexpr auto operator()(Type) const
    {
      using M = typename Type::type;
      return hana::bool_<decltype(
        hana::equal(
          hana::decltype_(nbdl::message::get_channel(std::declval<M>())),
          hana::type_c<nbdl::message::channel::upstream>
        )
      )::value>{};
    }
  };

  struct helper_fn
  {
    template<typename UpstreamMessages, typename DownstreamMessages>
    constexpr auto operator()(UpstreamMessages, DownstreamMessages) const
    {
      // To this point we are working with two lists of types
      // TODO: see if using `hana::types` would be possible
      // return fn(system_message_type) -> Product of variants
      return hana::demux(hana::make_pair)
      (
        hana::partial(builder::make_message_variant, UpstreamMessages{}),
        hana::partial(builder::make_message_variant, DownstreamMessages{})
      );
    } 
  };

  template<typename EntityMap, typename ProvidersMeta>
  constexpr auto operator()(EntityMap entity_map, ProvidersMeta providers) const
  {
    // return a Metafunction that 
    // takes a SystemMessage type and returns
    // a Product of the upstream/downstream
    // message nbdl::variants
    return hana::unpack(
      hana::partition(
        hana::unpack(
          hana::flatten(hana::unpack(providers,
            mpdef::make_list ^hana::on^ provider_meta::access_points)),
          hana::partial(builder::entity_messages, entity_map)
        ),
        is_upstream_helper_fn{}
      ),
      helper_fn{}
    );
  }
};
constexpr make_message_variants_fn make_message_variants{};

}//builder
}//nbdl_def
#endif
