#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#

if (EMSCRIPTEN)
  add_subdirectory(binders/js)
  add_subdirectory(core)
  add_subdirectory(js)
  add_subdirectory(websocket_api)
  add_subdirectory(webui)
  return()
endif (EMSCRIPTEN)

if (NBDL_HAS_UIKIT)
  add_subdirectory(ui_view)
  return()
endif (NBDL_HAS_UIKIT)

add_subdirectory(core)
add_subdirectory(util)
add_subdirectory(websocket)
add_subdirectory(binders/jsoncpp)
add_subdirectory(builder)
add_subdirectory(sql_db)
add_subdirectory(ui_helper)
add_subdirectory(webui)
add_subdirectory(websocket_api)
