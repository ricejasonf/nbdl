FROM nbdl_build:web_develop

  # Debug just doesn't work with emscripten right now
  ARG BUILD_TYPE=Release
  ENV BUILD_TYPE ${BUILD_TYPE}

  WORKDIR /opt/build
  CMD cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_TOOLCHAIN_FILE='/opt/toolchain.cmake' \
    /opt/src \
    && /bin/bash
