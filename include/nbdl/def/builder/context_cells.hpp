//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_CELLS_HPP
#define NBDL_DEF_BUILDER_CONTEXT_CELLS_HPP

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>

namespace nbdl_def {
namespace builder {

namespace hana  = boost::hana;
namespace hanax = boost::hana::experimental;

// return producer lookup, consumer lookup, and tuple of the instances of those
struct context_cells_fn
{
  template<typename ProducerKeys>
  struct make_producer_lookup_pair_list_fn
  {
    template<typename Index>
    constexpr auto operator()(Index i) const
    {
      return hana::unpack(hana::at(ProducerKeys{}, i), mpdef::make_list ^hana::on^
        hana::reverse_partial(mpdef::make_pair, i));
    }
  };

  template<typename ConsumerKeys>
  struct make_consumer_lookup_pair_fn
  {
    template<typename Index>
    constexpr auto operator()(Index i) const
    {
      return mpdef::make_pair(hana::at(ConsumerKeys{}, i), i);
    }
  };

  struct make_cell_tags_fn
  {
    template<typename ...ProducerTags, typename ...ConsumerTags>
    constexpr auto operator()(mpdef::list<ProducerTags...>, mpdef::list<ConsumerTags...>)
    {
      return hana::type_c<
        hanax::types<hana::tag_of_t<ProducerTags>..., hana::tag_of_t<ConsumerTags>...>
      >;
    }
  };

  // returns a pair.. (lookup, tuple type)
  template<typename ProducerMap, typename ConsumerMap>
  constexpr auto operator()(ProducerMap, ConsumerMap) const
  {
    using hana::on;
    // at this point the keys for the producer map
    // are a list of keys that should point to the
    // underlying producer instance
    constexpr auto pkeys = hana::unpack(ProducerMap{}, mpdef::make_list ^on^ hana::first);
    constexpr auto pvals = hana::unpack(ProducerMap{}, mpdef::make_list ^on^ hana::second);
    constexpr auto plookup = hana::decltype_(hana::unpack(
      hana::flatten(hana::unpack(
        hana::make_range(hana::size_c<0>, hana::length(pkeys)),
        mpdef::make_list ^on^ make_producer_lookup_pair_list_fn<decltype(pkeys)>{}
      )),
      mpdef::make_map
    ));
    constexpr auto cvals = hana::unpack(ConsumerMap{}, mpdef::make_list ^on^ hana::second);
#if 0
    // this stuff isn't used right now
    constexpr auto ckeys = hana::unpack(ConsumerMap{}, mpdef::make_list ^on^ hana::first);
    constexpr auto clookup = hana::decltype_(hana::unpack(
      hana::make_range(hana::size_c<0>, hana::length(ckeys)),
      mpdef::make_map ^on^ make_consumer_lookup_pair_fn<decltype(ckeys)>{}
    ));
#endif

    // the order should match the first three
    // template params of nbdl::Context
    return mpdef::make_list(plookup, make_cell_tags_fn{}(pvals, cvals));
  }
};
constexpr context_cells_fn context_cells{};

}//builder
}//nbdl_def
#endif
