#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_add_test("test.nbdl.builder.enumerate_access_points"
  enumerate_access_points.cpp
)

nbdl_add_test("test.nbdl.builder.access_point_meta"
  access_point_meta.cpp
)

nbdl_add_test("test.nbdl.builder.store"
  store.cpp
)

nbdl_add_test("test.nbdl.builder.store_map"
  store_map.cpp
)

nbdl_add_test("test.nbdl.builder.producer_meta"
  producer_meta.cpp
)

nbdl_add_test("test.nbdl.builder.producer_map"
  producer_map.cpp
)

nbdl_add_test("test.nbdl.builder.enumerate_producers"
  enumerate_producers.cpp
)

nbdl_add_test("test.nbdl.builder.enumerate_consumers"
  enumerate_consumers.cpp
)

nbdl_add_test("test.nbdl.builder.entity_messages"
  entity_messages.cpp
)

nbdl_add_test("test.nbdl.builder.message_api"
  message_api.cpp
)

nbdl_add_test("test.nbdl.builder.context"
  context.cpp
)
