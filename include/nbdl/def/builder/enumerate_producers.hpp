//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENUMERATE_PROVIDERS_HPP
#define NBDL_DEF_BUILDER_ENUMERATE_PROVIDERS_HPP

#include <mpdef/list.hpp>
#include <mpdef/tree_node.hpp>
#include <nbdl/def/builder/enumerate_access_points.hpp>
#include <nbdl/def/builder/producer_meta.hpp>
#include <nbdl/def/directives.hpp>

namespace nbdl_def {
namespace builder {

struct enumerate_producers_fn
{
  template<typename ProducerDefs>
  auto helper(ProducerDefs defs) const
  {
    return hana::unpack(defs,
      [](auto... producer_def) {
        return mpdef::make_list(builder::make_producer_meta(
          hana::second(producer_def)[tag::Type],
          hana::find(hana::second(producer_def), tag::Name)
            .value_or(hana::second(producer_def)[tag::Type]),
          builder::enumerate_access_points(producer_def)
        )...);
      }
    );
  }

  template<typename Def>
  constexpr auto operator()(Def) const
  {
    constexpr auto children = hana::second(Def{});
    constexpr auto single_producer = hana::transform(hana::find(children, tag::Producer),
        hana::partial(mpdef::make_tree_node, tag::Producer));
    constexpr auto producers = hana::find(children, tag::Producers);
    static_assert(hana::value(
      ((single_producer == hana::nothing) || (producers == hana::nothing))
      && hana::not_(single_producer == hana::nothing && producers == hana::nothing)
    ), "A definition of a Producer or Producers is required.");
    return decltype(
      helper(producers.value_or(
          hana::maybe(mpdef::make_list(), mpdef::make_list, single_producer)
        )
      )
    ){};
  }
};
constexpr enumerate_producers_fn enumerate_producers{};

}//builder
}//nbdl_def
#endif
