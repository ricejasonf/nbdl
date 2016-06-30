cmake_minimum_required(VERSION 2.8)

project(nbdl_project)
enable_testing()

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --exclude-regex "individual"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running high level tests."
)

function(nbdl_add_test name)
  add_executable("${name}" EXCLUDE_FROM_ALL ${ARGN})
  add_dependencies(check "${name}")
  if (NBDL_ENABLE_MEMCHECK)
    add_test(
      NAME ${name}
      COMMAND ${Valgrind_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${name} --leak-check=full --error-exitcode=1
    )
  else()
    add_test(
      NAME ${name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${name}
    )
  endif()
  add_custom_target(run.${name}
    COMMAND ./${name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build and run test: ${name}"
  )
  add_dependencies(run.${name} ${name})
endfunction()

function(nbdl_catch_test_suite out suite_name)
  set(_catch_main ${nbdl_project_SOURCE_DIR}/test/catch_main.cpp)
  foreach(_source IN ITEMS ${ARGN})
    string(REPLACE ".cpp" "" _name ${_source})
    list(APPEND _build_targets "individual.${suite_name}.${_name}")
    nbdl_add_test("individual.${suite_name}.${_name}" ${_catch_main} ${_source})
  endforeach()
  nbdl_add_test(${suite_name} ${_catch_main} ${ARGN})
  list(APPEND _build_targets "${suite_name}")
  set(${out} ${_build_targets} PARENT_SCOPE)
endfunction()

#todo find a cross platform way to set -Wall -Wextra
add_definitions(-std=c++1z)
#add_definitions(-ftemplate-backtrace-limit=0)
add_definitions(-Wall)
add_definitions(-Wextra)
if (${EMSCRIPTEN})
	add_definitions(--bind)
endif (${EMSCRIPTEN})

find_package(OpenSSL)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
#where did find boost go?
if (NBDL_AMAGLAMATED_JSONCPP_DIR)
  set(JsonCpp "${NBDL_AMAGLAMATED_JSONCPP_DIR}/dist/jsoncpp.cpp")
else (NBDL_AMAGLAMATED_JSONCPP_DIR)
  find_library(JsonCpp jsoncpp)
endif (NBDL_AMAGLAMATED_JSONCPP_DIR)

if (BOOST_HANA_INCLUDE_DIR)
  include_directories(BOOST_HANA_INCLUDE_DIR)
endif(BOOST_HANA_INCLUDE_DIR)

include_directories(${nbdl_project_SOURCE_DIR}/src)
include_directories(${nbdl_project_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
