//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_ENUMERATE_PROVIDERS_HPP
#define NBDL_DEF_BUILDER_ENUMERATE_PROVIDERS_HPP

#include<def/builder/EnumerateAccessPoints.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<def/directives.hpp>
#include<mpdef/List.hpp>
#include<mpdef/TreeNode.hpp>

namespace nbdl_def {
namespace builder {

struct enumerate_providers_fn
{
  template<typename ProviderDefs>
  auto helper(ProviderDefs defs) const
  {
    return hana::unpack(defs,
      [](auto... provider_def) {
        return mpdef::make_list(builder::make_provider_meta(
          hana::second(provider_def)[tag::Type],
          hana::find(hana::second(provider_def), tag::Name)
            .value_or(hana::second(provider_def)[tag::Type]),
          builder::enumerate_access_points(provider_def)
        )...);
      }
    );
  }

  template<typename Def>
  constexpr auto operator()(Def) const
  {
    constexpr auto children = hana::second(Def{});
    constexpr auto single_provider = hana::transform(hana::find(children, tag::Provider),
        hana::partial(mpdef::make_tree_node, tag::Provider));
    constexpr auto providers = hana::find(children, tag::Providers);
    static_assert(hana::value(
      ((single_provider == hana::nothing) || (providers == hana::nothing))
      && hana::not_(single_provider == hana::nothing && providers == hana::nothing)
    ), "A definition of a Provider or Providers is required.");
    return decltype(
      helper(providers.value_or(
          hana::maybe(mpdef::make_list(), mpdef::make_list, single_provider)
        )
      )
    ){};
  }
};
constexpr enumerate_providers_fn enumerate_providers{};

}//builder
}//nbdl_def
#endif
