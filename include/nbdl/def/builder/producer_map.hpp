//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_PROVIDER_MAP_HPP
#define NBDL_DEF_BUILDER_PROVIDER_MAP_HPP

#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/producer_meta.hpp>
#include <mpdef/pair.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace nbdl_def {
namespace builder {

namespace details {

  struct build_producer_pair_fn
  {
    template<typename ProducerMeta>
    constexpr auto operator()(ProducerMeta)
    {
      constexpr ProducerMeta producer_meta{};
      auto path_types = hana::unique(hana::transform(producer_meta::access_points(producer_meta),
        access_point_meta::path));
      return mpdef::make_pair(
        hana::append(path_types, producer_meta::name(producer_meta)),
        producer_meta::producer(producer_meta)
      );
    }
  };

}//details

struct producer_map_fn
{
  template<typename Producers>
  constexpr auto operator()(Producers producers) const
  {
    return hana::unpack(producers,
      hana::on(
        mpdef::make_map,
        details::build_producer_pair_fn{}
      )
    );
  }
};
constexpr producer_map_fn producer_map{};

}//builder
}//nbdl_def
#endif
