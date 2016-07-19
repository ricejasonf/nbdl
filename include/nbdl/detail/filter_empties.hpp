//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_FILTER_EMPTIES_HPP
#define NBDL_DETAIL_FILTER_EMPTIES_HPP

#include <boost/hana/bool.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/functional/on.hpp>
#include <boost/hana/tuple.hpp>
#include <functional>
#include <type_traits>
#include <utility>

namespace nbdl { namespace detail
{
  namespace hana = boost::hana;

  struct unref_fn
  {
    template <typename Ref>
    decltype(auto) operator()(Ref ref) const
    {
      return ref.get();
    }
  };

  template <typename F>
  struct filter_empties_fn
  {
    F f;
    template <typename ...X>
    decltype(auto) operator()(X&& ...x) const
    {
      return hana::unpack(
        hana::filter(hana::make_tuple(std::ref(x)...), [](auto ref)
        {
          return hana::bool_c<!std::is_empty<typename decltype(ref)::type>::value>;
        }),
        hana::on(f, detail::unref_fn{})
      );
    }
  };
}} // nbdl::detail

#endif

