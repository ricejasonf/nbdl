//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_MAKE_MESSAGE_API_HPP
#define NBDL_DEF_BUILDER_MAKE_MESSAGE_API_HPP

#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/def/builder/entity_messages.hpp>
#include <nbdl/def/builder/provider_meta.hpp>

#include <boost/hana/functional/on.hpp>
#include <boost/hana/functional/partial.hpp>
#include <boost/hana/flatten.hpp>
#include <boost/hana/partition.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>

namespace nbdl_def::builder
{

  namespace hana = boost::hana;

  template <
    typename UpstreamMessageTypes,
    typename DownstreamMessageTypes
  >
  struct message_api_meta
  {
    using upstream_types    = UpstreamMessageTypes;
    using downstream_types  = DownstreamMessageTypes;
  };

  struct make_message_api_fn
  {
    // aggregate all entity messages
    // and partition by upstream/downstream
    template<typename ProvidersMeta>
    constexpr auto operator()(ProvidersMeta providers) const
    {
      return hana::unpack(
        hana::partition(
          hana::flatten(
            hana::transform(
              hana::flatten(
                hana::unpack(providers,
                mpdef::make_list ^hana::on^ provider_meta::access_points)
              ),
              builder::entity_messages
            )
          ),
          hana::trait<nbdl::UpstreamMessage>
        ),
        hana::on(hana::template_<message_api_meta>, hana::decltype_)
      );
    }
  };
  constexpr make_message_api_fn make_message_api{};

}

#endif
