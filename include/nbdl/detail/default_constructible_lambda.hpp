//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_DEFAULT_CONSTRUCTIBLE_LAMBDA_HPP
#define NBDL_DETAIL_DEFAULT_CONSTRUCTIBLE_LAMBDA_HPP

#include <type_traits>

namespace nbdl::detail
{
  // Thanks, Paul Fultz II

  template <typename Lambda>
  struct default_constructible_lambda
  {
    static_assert(
      std::is_empty<Lambda>::value
    , "Default constructible lambda must not capture."
    );

    template <typename ...Xs>
    constexpr auto operator()(Xs&& ...xs) const
    {
      return reinterpret_cast<const Lambda&>(*this)(
        std::forward<Xs>(xs)...
      );
    }
  };
}

#endif
