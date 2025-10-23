//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_EXTRAS_HPP
#define NBDL_CONCEPT_EXTRAS_HPP

#include <concepts>
#include <type_traits>

namespace nbdl {
template <typename T, typename U>
concept SameAs = std::same_as<std::remove_cvref_t<T>,
                              std::remove_cvref_t<U>>;
}

#endif

