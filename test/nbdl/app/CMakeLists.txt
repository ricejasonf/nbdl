#
# Copyright Jason Rice 2019
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

nbdl_catch_test_suite(build_targets "test.nbdl.app"
 detail/cookie_parser.cpp)

nbdl_add_dom_test("test.nbdl.app.server_client" server_client.cpp)
if (NOT EMSCRIPTEN)
target_link_libraries("test.nbdl.app.server_client"
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${JsonCpp})
endif()

