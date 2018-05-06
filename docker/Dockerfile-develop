FROM nbdl_build:develop

  ARG BUILD_TYPE=Debug
  ENV BUILD_TYPE ${BUILD_TYPE}

  WORKDIR /opt/build
  CMD cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_TOOLCHAIN_FILE='/opt/toolchain.cmake' \
    /opt/src \
    && /bin/bash
