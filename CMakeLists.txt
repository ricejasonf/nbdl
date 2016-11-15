cmake_minimum_required(VERSION 2.8)

project(nbdl_project)
enable_testing()

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --exclude-regex "individual"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running high level tests."
)

#TODO require
set(dom_test_dir ${CMAKE_BINARY_DIR}/tool/dom_test)
add_subdirectory(tool/dom_test)
if (EMSCRIPTEN)
  set(NBDL_TEST_COMMAND "node")
endif(EMSCRIPTEN)

function(nbdl_add_test name_)
  string(REPLACE "/" "." name ${name_})
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
      COMMAND ${NBDL_TEST_COMMAND} ./${name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif()
  add_custom_target(run.${name}
    COMMAND ${NBDL_TEST_COMMAND} ./${name}
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

function(nbdl_add_dom_test name_)
  string(REPLACE "/" "." name ${name_})
  if (NOT EMSCRIPTEN)
    return()
  endif(NOT EMSCRIPTEN)
  add_executable("${name}" EXCLUDE_FROM_ALL ${ARGN})
  add_dependencies(check "${name}")
  add_dependencies(${name} tool.dom_test)
  get_target_property(link_flags_ ${name} LINK_FLAGS)
  set_target_properties(${name} PROPERTIES LINK_FLAGS
    "${link_flags} --bind -s ASSERTIONS=1 --memory-init-file 0"
  )
  add_test(
    NAME ${name}
    COMMAND ${NBDL_TEST_COMMAND} ${dom_test_dir}/index.js ./${name}.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  add_custom_target(run.${name}
    COMMAND ${NBDL_TEST_COMMAND} ${dom_test_dir}/index.js ./${name}.js
    DEPENDS ${name} tool.dom_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build and run test: ${name}"
  )
endfunction()

function(nbdl_catch_dom_test_suite out suite_name)
  if (NOT EMSCRIPTEN)
    return()
  endif(NOT EMSCRIPTEN)
  set(_catch_main ${nbdl_project_SOURCE_DIR}/test/catch_main.cpp)
  foreach(_source IN ITEMS ${ARGN})
    string(REPLACE ".cpp" "" _name ${_source})
    list(APPEND _build_targets "individual.${suite_name}.${_name}")
    nbdl_add_dom_test("individual.${suite_name}.${_name}" ${_catch_main} ${_source})
  endforeach()
  nbdl_add_dom_test(${suite_name} ${_catch_main} ${ARGN})
  list(APPEND _build_targets "${suite_name}")
  set(${out} ${_build_targets} PARENT_SCOPE)
endfunction()

#todo find a cross platform way to set -Wall -Wextra
add_definitions(-std=c++1z)
add_definitions(-ftemplate-backtrace-limit=0)
add_definitions(-Wall)
add_definitions(-Wextra)

if (${NBDL_USE_CATCH})
  add_definitions(-DNBDL_USE_CATCH)
endif (${NBDL_USE_CATCH})

if (${EMSCRIPTEN})
	add_definitions(--bind)
endif (${EMSCRIPTEN})

if (NBDL_AMAGLAMATED_JSONCPP_DIR)
  set(JsonCpp "${NBDL_AMAGLAMATED_JSONCPP_DIR}/dist/jsoncpp.cpp")
else (NBDL_AMAGLAMATED_JSONCPP_DIR)
  find_library(JsonCpp jsoncpp)
endif (NBDL_AMAGLAMATED_JSONCPP_DIR)

if (BOOST_HANA_INCLUDE_DIR)
  include_directories(BOOST_HANA_INCLUDE_DIR)
endif(BOOST_HANA_INCLUDE_DIR)

include_directories(${nbdl_project_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(example/tiles)
