//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<assets/TestContext.hpp>
#include<def/builder/Context.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  // these are just constructor smoke tests
  {
    // test default construction
    auto def = test_context_def::make(
      hana::template_<test_context::Provider>,
      hana::template_<test_context::Provider>,
      hana::template_<test_context::Consumer>,
      hana::template_<test_context::Consumer>
    );
    using Context = test_context_def::make_context_t<decltype(def)>;
    Context();
  }
  {
    using test_context::IntTag;
    // test single parameter construction
    auto def = test_context_def::make(
      hana::reverse_partial(hana::template_<test_context::Provider>, hana::type_c<IntTag<1>>),
      hana::reverse_partial(hana::template_<test_context::Provider>, hana::type_c<IntTag<2>>),
      hana::reverse_partial(hana::template_<test_context::Consumer>, hana::type_c<IntTag<3>>),
      hana::reverse_partial(hana::template_<test_context::Consumer>, hana::type_c<IntTag<4>>)
    );
    using Context = test_context_def::make_context_t<decltype(def)>;
    Context(IntTag<1>{}, IntTag<2>{}, IntTag<3>{}, IntTag<4>{});
  }
  // TODO: construct a context with a hana::map
}
