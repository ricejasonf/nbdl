set(CMAKE_SYSROOT /opt/sysroot)
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(triple x86_64-apple-tvossimulator)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_AR /usr/local/bin/ar CACHE FILEPATH "Archiver")
set(CMAKE_STRIP /usr/local/bin/strip CACHE FILEPATH "Archiver")

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
