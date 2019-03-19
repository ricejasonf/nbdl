//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_CONTEXT_HPP
#define NBDL_MAKE_CONTEXT_HPP

#include <nbdl/def/builder/context.hpp>
#include <mpdef/utility.hpp>
#include <nbdl/context.hpp>

#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/mp11/list.hpp>
#include <memory>
#include <utility>

namespace nbdl
{
  // deprecated (still used in tests)
  template <typename Tag, typename ...Args>
  constexpr decltype(auto) make_unique_context(Args&& ...args)
  {
    using Context = nbdl::context<Tag>;
    return std::make_unique<Context>(std::forward<Args>(args)...);
  }

  // actor - named pair operand for use with make_context
  using actor(using auto name, using auto arg)
  {
    return hana::make_pair(mpdef::to_constant(name), arg);
  }

  namespace make_context_detail
  {
    template <typename Context, typename Names, typename Params, auto ...i>
    auto make_helper(Params&& params, std::index_sequence<i...>)
    {
      return std::make_unique<Context>(
        hana::find(params, mp_at_c<Names, i>{})
          .value_or(hana::type_c<void>)...);
    }
  }

  // make_context - named parameter interface for making a context
  //                Use `actor("name") = arg` for named_pairs
  template <typename Tag>
  constexpr auto make_context = [](auto&& ...named_pairs)
  {
    using ActorNames = typename nbdl_def::builder::make_context_meta_t<Tag>
                                                 ::actor_names;
    auto params = hana::make_map(
        std::forward<decltype(named_pairs)>(named_pairs)...);

    return make_context_detail::make_helper<nbdl::context<Tag>, ActorNames>(
      std::move(params)
    , std::make_index_sequence<mp_size<ActorNames>{}>{}
    );

#if 0 // causes ICE
    return std::make_unique<nbdl::context<Tag>>(
      std::move(params[
        mp_at_c<ActorNames, mpdef::iota~(N)>{}
      ]).value_or(hana::type_c<void>)
      ...
    );
#endif
  };
}

#endif
