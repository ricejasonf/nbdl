//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_CONTEXT_CELLS_HPP
#define NBDL_DEF_BUILDER_CONTEXT_CELLS_HPP

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

// return provider lookup, consumer lookup, and tuple of the instances of those
struct ContextCells
{
  template<typename ProviderKeys>
  struct MakeProviderLookupPairList
  {
    template<typename Index>
    constexpr auto operator()(Index i) const
    {
      return hana::unpack(ProviderKeys{}, mpdef::make_list ^hana::on^
        hana::reverse_partial(mpdef::make_pair, i));
    }
  };

  template<typename ConsumerKeys>
  struct MakeConsumerLookupPair
  {
    template<typename Index>
    constexpr auto operator()(Index i) const
    {
      return mpdef::make_pair(hana::at(ConsumerKeys{}, i), i);
    }
  };

  template<typename ProviderMfs, typename ConsumerMfs>
  struct MakeContextCells
  {
    template<typename PushUpstreamFnType, typename PushDownstreamFnType>
    constexpr auto operator()(PushUpstreamFnType, PushDownstreamFnType)
    {
      return hana::unpack(
        hana::concat(
          hana::ap(ProviderMfs{}, mpdef::make_list(PushDownstreamFnType{})),
          hana::ap(ConsumerMfs{}, mpdef::make_list(PushUpstreamFnType{}))
        ),
        hana::template_<hana::tuple>
      );
    }
  };

  // returns a pair.. (lookup, tuple type)
  template<typename ProviderMap, typename ConsumerMap>
  constexpr auto operator()(ProviderMap, ConsumerMap) const
  {
    using hana::on;
    // at this point the keys for the provider map
    // are a list of keys that should point to the
    // underlying provider instance
    constexpr auto pkeys = hana::unpack(ProviderMap{}, mpdef::make_list ^on^ hana::first);
    constexpr auto pvals = hana::unpack(ProviderMap{}, mpdef::make_list ^on^ hana::second);
    constexpr auto plookup = hana::decltype_(hana::unpack(
      hana::flatten(hana::unpack(
        hana::make_range(hana::size_c<0>, hana::length(pkeys)),
        mpdef::make_list ^on^ MakeProviderLookupPairList<decltype(pkeys)>{}
      )),
      mpdef::make_map
    ));
    constexpr auto ckeys = hana::unpack(ConsumerMap{}, mpdef::make_list ^on^ hana::first);
    constexpr auto cvals = hana::unpack(ConsumerMap{}, mpdef::make_list ^on^ hana::second);
    constexpr auto clookup = hana::decltype_(hana::unpack(
      hana::make_range(hana::size_c<0>, hana::length(ckeys)),
      mpdef::make_map ^on^ MakeConsumerLookupPair<decltype(ckeys)>{}
    ));

    constexpr auto make_context_cells = hana::type_c<MakeContextCells<decltype(pvals), decltype(cvals)>>;

    // the order should match the first three
    // template params of nbdl::Context
    return mpdef::make_list(plookup, clookup, make_context_cells);
  }
};
constexpr ContextCells contextCells{};

}//builder
}//nbdl_def
#endif
