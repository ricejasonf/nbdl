cmake_minimum_required(VERSION 2.8)
set(NBDL_TEST 1)

project(nbdl_project)

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

#CMAKE_CXX_STANDARD 14 doesn't work until cmake 3.1+
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

#todo find a cross platform way to set -Wall -Wextra
add_definitions(-std=c++14)
add_definitions(-Wall)
add_definitions(-Wextra)
if (${EMSCRIPTEN})
	add_definitions(--bind)
endif (${EMSCRIPTEN})

if (${EMSCRIPTEN})
else (${EMSCRIPTEN})
  find_package(OpenSSL REQUIRED)
  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED OFF)
  set(Boost_USE_STATIC_RUNTIME OFF)
  #find_package(Boost COMPONENTS regex REQUIRED)
  if (NBDL_AMAGLAMATED_JSONCPP_DIR)
    set(JsonCpp "${NBDL_AMAGLAMATED_JSONCPP_DIR}/dist/jsoncpp.cpp")
  else (NBDL_AMAGLAMATED_JSONCPP_DIR)
    find_library(JsonCpp jsoncpp)
  endif (NBDL_AMAGLAMATED_JSONCPP_DIR)
endif (${EMSCRIPTEN})

add_subdirectory(src)
if (NBDL_TEST)
  add_subdirectory(test/src)
endif (NBDL_TEST)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake")