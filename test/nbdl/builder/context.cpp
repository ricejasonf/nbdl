//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <def/builder/Context.hpp>
#include <nbdl/make_context.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  // these are just constructor smoke tests
  {
    // test default construction
    constexpr auto def = test_context_def::make(
      test_context::provider_tag{},
      test_context::provider_tag{},
      test_context::consumer_tag{},
      test_context::consumer_tag{}
    );
    auto context_ptr = nbdl::make_unique_context(def);
  }
  {
    using test_context::int_tag;
    constexpr auto def = test_context_def::make(
      test_context::provider_tag{},
      test_context::provider_tag{},
      test_context::consumer_tag{},
      test_context::consumer_tag{}
    );
    // test single parameter construction
    auto context_ptr = nbdl::make_unique_context(
      def,
      int_tag<1>{},
      int_tag<2>{},
      int_tag<3>{},
      int_tag<4>{}
    );
  }
  // It would be nice to be able to
  // construct a context with named params.
}
