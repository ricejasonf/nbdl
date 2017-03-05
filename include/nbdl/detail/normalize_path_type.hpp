//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_NORMALIZE_PATH_TYPE_HPP
#define NBDL_DETAIL_NORMALIZE_PATH_TYPE_HPP

#include <boost/hana.hpp>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  // Idempotently converts a "create path" into the
  // same type as the path to the object it creates
  // so it can be found in type maps.
  //
  // The representation of paths is implementation defined.
  // (
  //   Currently it's just a tuple where a "create path"
  //   has the last node wrapped as a hana::type.
  // )
  template <typename Path>
  struct normalize_path
  {
    using LastKey = typename decltype(hana::typeid_(hana::back(std::declval<Path>())))::type;
    using type = decltype(
      hana::append(
        hana::drop_back(std::declval<Path>(), hana::int_c<1>),
        std::declval<LastKey>()
      )
    );
  };

  constexpr auto normalize_path_type = hana::metafunction<normalize_path>;
}

#endif
