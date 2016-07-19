//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_JUSTIFY_TYPE_HPP
#define NBDL_MPDEF_JUSTIFY_TYPE_HPP

#include <boost/hana/optional.hpp>
#include <boost/hana/type.hpp>

namespace mpdef {

// JustifyType - metafunction that transforms an optional
// `type` into its contained type
template<typename T>
struct justify_type_t;

template<>
struct justify_type_t<boost::hana::optional<>>
{
  using type = const boost::hana::optional<>;
};

template<typename Type>
struct justify_type_t<boost::hana::optional<Type>>
{
  using type = boost::hana::optional<typename Type::type>;
};

// specializations for const :/
template<>
struct justify_type_t<const boost::hana::optional<>>
{
  using type = const boost::hana::optional<>;
};

template<typename Type>
struct justify_type_t<const boost::hana::optional<Type>>
{
  using type = boost::hana::optional<typename Type::type>;
};

constexpr auto justify_type = boost::hana::metafunction<mpdef::justify_type_t>;

}//mpdef

#endif
