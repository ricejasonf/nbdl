//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/consumer_meta.hpp>
#include <nbdl/def/builder/enumerate_consumers.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Consumer1);
DEFINE_NAME(Consumer2);

DEFINE_NAME(Consumer1Name);
DEFINE_NAME(Consumer2Name);

}//names

int main()
{
  using namespace nbdl_def;
  using builder::consumer_meta;
  {

    constexpr auto def =
      Context(
        Consumers(
          Consumer(
            Name(names::Consumer1Name),
            Type(names::Consumer1)
          ),
          Consumer(
            Name(names::Consumer2Name),
            Type(names::Consumer2)
          )
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_consumers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_consumer_meta_with_map(
          consumer_meta::consumer    = names::Consumer1,
          consumer_meta::name        = names::Consumer1Name
        ),
        builder::make_consumer_meta_with_map(
          consumer_meta::consumer    = names::Consumer2,
          consumer_meta::name        = names::Consumer2Name
        )
      )
    );
  }

  // define a single consumer
  {
    constexpr auto def =
      Context(
        Consumer(
          Name(names::Consumer1Name),
          Type(names::Consumer1)
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_consumers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_consumer_meta_with_map(
          consumer_meta::consumer    = names::Consumer1,
          consumer_meta::name        = names::Consumer1Name
        )
      )
    );
  }
}

