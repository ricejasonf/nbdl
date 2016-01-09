//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_APPEND_IF_HPP
#define NBDL_MPDEF_APPEND_IF_HPP

#include<boost/hana.hpp>
#include<utility>

namespace mpdef {

namespace hana = boost::hana;

namespace details {

  template<bool cond>
  struct AppendIfHelper;

  template<>
  struct AppendIfHelper<true>
  {
    template<typename State, typename X>
    static constexpr auto apply(State&& state, X&& x)
    {
      return hana::append(std::forward<State>(state), std::forward<X>(x));
    }
  };

  template<>
  struct AppendIfHelper<false>
  {
    template<typename State, typename X>
    static constexpr auto apply(State&& state, X)
    {
      return std::forward<State>(state);
    }
  };

}//details

struct AppendIf
{
  template<typename Pred, typename State, typename X>
  constexpr auto operator()(Pred, State&& state, X&& x) const
  {
    return details::AppendIfHelper<
      static_cast<bool>(std::decay_t<decltype(std::declval<Pred>()(std::declval<X>()))>::value)
    >::apply(std::forward<State>(state), std::forward<X>(x));
  }
};
constexpr auto appendIf = hana::curry<3>(AppendIf{});

}//mpdef
#endif
