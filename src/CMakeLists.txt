#
# Copyright Jason Rice 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 2.8)

if (EMSCRIPTEN)
	add_library(nbdl_regex STATIC 
		Regex.cpp
		)
  return()
endif (EMSCRIPTEN)

add_library(nbdl_regex STATIC 
  Regex.cpp
  )
add_library(nbdl_servers_ws STATIC 
  servers/ws/HttpRequestParser.cpp
  servers/ws/HttpResponse.cpp
  servers/ws/MessageParser.cpp
  servers/ws/MessageGenerator.cpp
  )

foreach(t IN LISTS SKIP_TARGETS)
  set_target_properties(${t} PROPERTIES EXCLUDE_FROM_ALL TRUE)
endforeach()
