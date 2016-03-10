//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_PROCESS_MESSAGE_HPP
#define NBDL_FWD_PROCESS_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename = void>
  struct dispatch_impl : dispatch_impl<T, hana::when<true>> { };

  struct dispatch_fn
  {
    template<typename Store, typename Message>
    constexpr auto operator()(Store&&, Message&&) const;
  };

  constexpr dispatch_fn dispatch{};
} // nbdl

#endif
