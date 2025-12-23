//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXT_STD_TUPLE_LIKE_HPP
#define NBDL_EXT_STD_TUPLE_LIKE_HPP

#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/State.hpp>
#include <nbdl/tags.hpp>

#include <tuple>

namespace nbdl {
  template <typename T>
  concept StdTupleLike = requires {
    std::tuple_size<std::remove_cvref_t<T>>();
  };

  template <StdTupleLike T>
  struct get_impl<T> {
    template <typename Store, size_t i>
    static decltype(auto) apply(Store&& s, index_t<i>) {
      return std::get<i>(std::forward<Store>(s));
    }

    template <typename Store, typename U>
    static decltype(auto) apply(Store&& s, type_t<U>) {
      return std::get<U>(std::forward<Store>(s));
    }
  };

  template <StdTupleLike Tag>
  struct bind_sequence_impl<Tag> {
    template <typename BindableSequence, typename BindFn>
    static constexpr auto apply(BindableSequence&& xs, BindFn&& f) {
      auto&& [...x] = std::forward<BindableSequence>(xs);
      return std::forward<BindFn>(f)(std::forward<decltype(x)>(x)...);
    }
  };
}

#endif
