//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_CONCEPT_PRED_HPP
#define NBDL_DETAIL_CONCEPT_PRED_HPP

#include <boost/hana/bool.hpp>

namespace nbdl { namespace detail
{
  namespace hana = boost::hana;

  template <template <typename ...> class Concept>
  struct concept_pred_fn
  {
    template <typename T>
    constexpr auto operator()(T const&) const
      -> hana::bool_<Concept<T>::value>
    { return {}; }
  };

  template <template <typename ...> class Concept>
  constexpr concept_pred_fn<Concept> concept_pred{};
}} // nbdl::detail

#endif


