#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if (EMSCRIPTEN)
  add_subdirectory(core)
  add_subdirectory(webui)
  return()
endif (EMSCRIPTEN)

add_subdirectory(core)
add_subdirectory(websocket)
add_subdirectory(binders/jsoncpp)
add_subdirectory(builder)
