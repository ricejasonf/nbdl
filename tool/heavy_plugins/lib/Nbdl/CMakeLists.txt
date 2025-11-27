cmake_minimum_required(VERSION 3.13.4)

add_library(Nbdl MODULE Nbdl.cpp
                        NbdlTranslateCpp.cpp
                        NbdlDialect.cpp)
target_compile_options(Nbdl PRIVATE -fno-rtti -fno-exceptions)
add_dependencies(Nbdl NbdlOpsIncGen)
install(TARGETS Nbdl
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
