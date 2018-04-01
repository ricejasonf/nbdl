//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_CONTEXT_HPP
#define NBDL_MAKE_CONTEXT_HPP

//#include <def/builder/Context.hpp>
#include <nbdl/context.hpp>

#include <memory>
#include <utility>

namespace nbdl
{
  namespace hanax = boost::hana::experimental;

  template <typename Tag, typename ...Args>
  constexpr decltype(auto) make_unique_context(Args&& ...args)
  {
    using Context = nbdl::context<Tag>;
    return std::make_unique<Context>(std::forward<Args>(args)...);
  }
} // nbdl

#endif
