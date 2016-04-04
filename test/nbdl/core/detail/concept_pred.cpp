//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<nbdl/detail/concept_pred.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

template <typename T>
struct TestConcept
{
  static constexpr bool value = std::is_integral<T>::value;
};

int main()
{
  // converts a concept into a high order predicate function
  // that can be passed as an argument
  constexpr auto result = hana::transform(
    hana::make_tuple(6, false, 4.6f),
    nbdl::detail::concept_pred<TestConcept>
  );
  constexpr auto expected = hana::make_tuple(hana::true_c, hana::true_c, hana::false_c);

  BOOST_HANA_CONSTANT_ASSERT(result == expected);
}
