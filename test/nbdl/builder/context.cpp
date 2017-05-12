//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <nbdl/def/builder/context.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/make_def.hpp>
#include <nbdl/null_store.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

namespace
{
  struct builder_test_context { }; 
}

namespace nbdl
{
  template <>
  struct make_def_impl<builder_test_context>
  {
    static constexpr auto apply()
    {
      return test_context_def::make(
        test_context::producer_tag{},
        test_context::producer_tag{},
        test_context::consumer_tag{},
        test_context::consumer_tag{},
        nbdl::null_store{}
      );
    }
  };
}

int main()
{
  // these are just constructor smoke tests
  {
    auto context_ptr = nbdl::make_unique_context<builder_test_context>();
  }
  {
    using test_context::int_tag;
    // test single parameter construction
    auto context_ptr = nbdl::make_unique_context<builder_test_context>(
      int_tag<1>{},
      int_tag<2>{},
      int_tag<3>{},
      int_tag<4>{}
    );
  }
  {
    using test_context::int_tag;
    // test single parameter construction
    auto context_ptr = nbdl::make_unique_context<builder_test_context>(
      int_tag<1>{},
      hana::type_c<void>,
      int_tag<3>{},
      int_tag<4>{}
    );
  }
}
