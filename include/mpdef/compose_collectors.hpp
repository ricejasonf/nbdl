//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_COMPOSE_COLLECTORS_HPP
#define NBDL_MPDEF_COMPOSE_COLLECTORS_HPP

#include <mpdef/list.hpp>

#include <boost/hana.hpp>
#include <utility>

namespace mpdef {

namespace hana = boost::hana;

struct compose_collectors_fn
{
  template<std::size_t... i, typename... Collector>
  constexpr auto helper(std::index_sequence<i...>, Collector&&... collector) const
  {
    return hana::demux(mpdef::make_list)
    (
      hana::demux(std::forward<Collector>(collector))
      (
        hana::demux(hana::reverse_partial(hana::at, hana::int_c<i>))(hana::arg<1>),
        hana::arg<2>
      )...
    );
  }

  template<typename... Collector>
  constexpr auto operator()(Collector&&... collector) const
  {
    return helper(std::make_index_sequence<sizeof...(Collector)>{}, 
        std::forward<Collector>(collector)...);
  }
};
constexpr compose_collectors_fn compose_collectors{};

}//mpdef
#endif
