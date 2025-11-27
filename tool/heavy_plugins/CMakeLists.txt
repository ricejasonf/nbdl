cmake_minimum_required(VERSION 3.13.4)

project(nbdl_spec_project)

#find_package(HEAVY REQUIRED CONFIG)
find_package(MLIR REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
#list(APPEND CMAKE_MODULE_PATH "${HEAVY_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)
#include(AddHEAVY)
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_STANDARD 17)

# Nbdl Dialect and heavy-scheme Plugin
add_subdirectory(include/nbdl_spec)
add_subdirectory(lib/Nbdl)
