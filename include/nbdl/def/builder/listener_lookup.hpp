//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_BUILDER_LISTENER_LOOKUP_HPP
#define NBDL_DEF_BUILDER_LISTENER_LOOKUP_HPP

#include <nbdl/def/builder/producer_meta.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <mpdef/group_by.hpp>
#include <mpdef/list.hpp>
#include <mpdef/metastruct.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <type_traits>

namespace nbdl_def::builder
{
  using namespace boost::mp11;

  template <typename AccessPoint>
  struct listener_access_point_helper
  {
    using ValuePathType = std::decay_t<decltype(access_point_meta::path(AccessPoint{}))>;
    using ListenPaths = std::decay_t<decltype(access_point_meta::listen_paths(AccessPoint{}))>;

    using type = mp_transform_q<mp_bind_back<mpdef::list, mpdef::list<ValuePathType>>, ListenPaths>;
  };

  template <typename ProducersMeta>
  using listener_lookup = mpdef::group_by<
    mp_apply<mp_append,
    mp_transform_q<mp_quote_trait<listener_access_point_helper>,
    mp_apply<mp_append,
    mp_transform_q<mpdef::metastruct_get<decltype(producer_meta::access_points)>,
      std::decay_t<ProducersMeta> 
    >>>>
  >;
}

#endif
