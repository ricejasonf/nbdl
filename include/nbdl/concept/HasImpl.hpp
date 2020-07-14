//
// Copyright Jason Rice 2020
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_HAS_IMPL_HPP
#define NBDL_CONCEPT_HAS_IMPL_HPP

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/tag_of.hpp>

namespace nbdl {
  namespace hana = boost::hana;

  using default_impl = hana::default_;

  template<typename T, template <typename> typename Impl>
  concept HasImpl = !hana::is_default<Impl<hana::tag_of_t<T>>>::value;
}

#endif
