//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_HPP
#define NBDL_DEF_BUILDER_CONTEXT_HPP

#include <mpdef/list.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/def/builder/consumer_map.hpp>
#include <nbdl/def/builder/context_cells.hpp>
#include <nbdl/def/builder/enumerate_providers.hpp>
#include <nbdl/def/builder/enumerate_consumers.hpp>
#include <nbdl/def/builder/message_api.hpp>
#include <nbdl/def/builder/provider_map.hpp>
#include <nbdl/def/builder/provider_meta.hpp>
#include <nbdl/def/builder/store_map.hpp>
#include <nbdl/make_def.hpp>

#include <boost/hana/concat.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/flatten.hpp>
#include <boost/hana/functional/on.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <utility>

namespace nbdl_def { namespace builder
{

  namespace hana  = boost::hana;
  namespace hanax = boost::hana::experimental;

  template <
    typename ProviderLookup,
    typename CellTagTypes,
    typename StoreMap,
    typename MessageApiMeta
  >
  struct context_meta
  {
    using provider_lookup       = ProviderLookup;
    using cell_tag_types        = CellTagTypes;
    using store_map             = StoreMap;
    using message_api_meta      = MessageApiMeta;
  };

  struct context_fn
  {
    template <typename TagType, typename ArgTypes>
    constexpr auto operator()(TagType, ArgTypes) const
    {
      using Def = decltype(::nbdl::make_def(TagType{}));

      static_assert(hana::first(Def{}) == tag::Context, "");

      constexpr auto providers_meta   = builder::enumerate_providers(Def{});
      constexpr auto consumers_meta   = builder::enumerate_consumers(Def{});
      constexpr auto provider_map     = builder::provider_map(providers_meta);
      constexpr auto consumer_map     = builder::consumer_map(consumers_meta);
      constexpr auto cell_info        = builder::context_cells(provider_map, consumer_map);
      constexpr auto store_map        = builder::store_map(
                                          hana::flatten(
                                            hana::unpack(providers_meta,
                                            mpdef::make_list ^hana::on^ provider_meta::access_points)
                                          )
                                        );
      constexpr auto message_api      = builder::make_message_api(providers_meta);
      constexpr auto params = hana::concat(
        cell_info,
        mpdef::make_list(store_map, message_api)
      );

      return hana::unpack(params, hana::template_<context_meta>);
    }
  };

  constexpr context_fn context{};

  template <typename Tag, typename ArgTypes>
  using make_context_meta_t = typename decltype(builder::context(hana::type_c<Tag>, ArgTypes{}))::type;
}} // nbdl_def::builder

#endif
