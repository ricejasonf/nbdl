//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONSUMER_MAP_HPP
#define NBDL_DEF_BUILDER_CONSUMER_MAP_HPP

#include<def/builder/ConsumerMeta.hpp>
#include<mpdef/Pair.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

struct consumer_map_fn
{
  template<typename ConsumersMeta>
  constexpr auto operator()(ConsumersMeta xs) const
  {
    return hana::unpack(xs,
      hana::on(
        mpdef::make_map,
        hana::demux(mpdef::make_pair)
        (
          consumer_meta::name,
          consumer_meta::consumer
        )
      )
    );
  }
};
constexpr consumer_map_fn consumer_map{};

}//builder
}//nbdl_def
#endif
