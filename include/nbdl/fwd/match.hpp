//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MATCH_HPP
#define NBDL_FWD_MATCH_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct match_impl : match_impl<T, hana::when<true>> { };

  struct match_fn
  {
    template<typename Store, typename Path, typename ...Fn>
    constexpr decltype(auto) operator()(Store&&, Path&&, Fn&&...) const;
  };

  constexpr match_fn match{};
} // nbdl

#endif
