//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENUMERATE_CONSUMERS_HPP
#define NBDL_DEF_BUILDER_ENUMERATE_CONSUMERS_HPP

#include <mpdef/list.hpp>
#include <mpdef/pair.hpp>
#include <nbdl/def/builder/consumer_meta.hpp>
#include <nbdl/def/directives.hpp>

#include <boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

struct enumerate_consumers_fn
{
  template<typename ConsumerDefs>
  auto helper(ConsumerDefs defs) const
  {
    return hana::unpack(defs,
      [](auto... consumer_def) {
        return mpdef::make_list(builder::make_consumer_meta(
          hana::second(consumer_def)[tag::Type],
          hana::find(hana::second(consumer_def), tag::Name)
            .value_or(hana::second(consumer_def)[tag::Type])
        )...);
      }
    );
  }

  template<typename Def>
  constexpr auto operator()(Def) const
  {
    constexpr auto children = hana::second(Def{});
    constexpr auto single_consumer = hana::transform(hana::find(children, tag::Consumer),
        hana::partial(mpdef::make_pair, tag::Consumer));
    constexpr auto consumers = hana::find(children, tag::Consumers);
    static_assert(hana::value(
      ((single_consumer == hana::nothing) || (consumers == hana::nothing))
      && hana::not_(single_consumer == hana::nothing && consumers == hana::nothing)
    ), "A definition of a Consumer or Consumers is required.");
    return decltype(
      helper(consumers.value_or(
          hana::maybe(mpdef::make_list(), mpdef::make_list, single_consumer)
        )
      )
    ){};
  }
};
constexpr enumerate_consumers_fn enumerate_consumers{};

}//builder
}//nbdl_def
#endif

