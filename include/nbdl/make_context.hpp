//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_CONTEXT_HPP
#define NBDL_MAKE_CONTEXT_HPP

#include <def/builder/Context.hpp>

#include <memory>
#include <utility>

namespace nbdl
{
#if 0 // NOT USED NOT TESTED
  template <typename ContextDef, typename ...Args>
  auto* new_context(ContextDef const& def, Args&& ...args)
  {
    using Context = typename decltype(
      nbdl_def::builder::make_context_type(def, std::forward<Args>(args)...)
    )::type;
    return new Context(std::forward<Args>(args)...);
  }

  template <typename ContextDef, typename ...Args>
  constexpr decltype(auto) make_shared_context(ContextDef const& def, Args&& ...args)
  {
    using Context = typename decltype(
      nbdl_def::builder::make_context_type(def, std::forward<Args>(args)...)
    )::type;
    return std::make_shared<Context>(std::forward<Args>(args)...);
  }
#endif // NOT USED NOT TESTED

  template <typename ContextDef, typename ...Args>
  constexpr decltype(auto) make_unique_context(ContextDef const& def, Args&& ...args)
  {
    using Context = typename decltype(
      nbdl_def::builder::make_context_type(def, std::forward<Args>(args)...)
    )::type;
    return std::make_unique<Context>(std::forward<Args>(args)...);
  }
} // nbdl

#endif
