//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/List.hpp>
#include<def/builder/ConsumerMeta.hpp>
#include<def/builder/EnumerateConsumers.hpp>

#include<boost/hana.hpp>

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
  using builder::ConsumerMeta;
  {

    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
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

    constexpr auto result = nbdl_def::builder::enumerateConsumers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::makeConsumerMetaWithMap(
          ConsumerMeta::consumer    = names::Consumer1,
          ConsumerMeta::name        = names::Consumer1Name
        ),
        builder::makeConsumerMetaWithMap(
          ConsumerMeta::consumer    = names::Consumer2,
          ConsumerMeta::name        = names::Consumer2Name
        )
      )
    );
  }

  // define a single consumer
  {
    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
        Consumer(
          Name(names::Consumer1Name),
          Type(names::Consumer1)
        )
      );

    constexpr auto result = nbdl_def::builder::enumerateConsumers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::makeConsumerMetaWithMap(
          ConsumerMeta::consumer    = names::Consumer1,
          ConsumerMeta::name        = names::Consumer1Name
        )
      )
    );
  }
}

