#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_add_test("test.nbdl.builder.enumerate_access_points"
  enumerate_access_points.cpp
)

nbdl_add_test("test.nbdl.builder.entity_meta"
  entity_meta.cpp
)

nbdl_add_test("test.nbdl.builder.access_point_meta"
  access_point_meta.cpp
)

nbdl_add_test("test.nbdl.builder.path"
  path.cpp
)

nbdl_add_test("test.nbdl.builder.store"
  store.cpp
)

nbdl_add_test("test.nbdl.builder.store_map"
  store_map.cpp
)

nbdl_add_test("test.nbdl.builder.provider_meta"
  provider_meta.cpp
)

nbdl_add_test("test.nbdl.builder.provider_map"
  provider_map.cpp
)

nbdl_add_test("test.nbdl.builder.enumerate_providers"
  enumerate_providers.cpp
)

nbdl_add_test("test.nbdl.builder.map_entity_meta"
  map_entity_meta.cpp
)

nbdl_add_test("test.nbdl.builder.entity_messages"
  entity_messages.cpp
)

nbdl_add_test("test.nbdl.builder.message_variants"
  message_variants.cpp
)

nbdl_add_test("test.nbdl.builder.context"
  context.cpp
)
