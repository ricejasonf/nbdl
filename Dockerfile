FROM ricejasonf/clang
WORKDIR /usr/local/src

# JsonCpp
RUN git clone https://github.com/open-source-parsers/jsoncpp.git \
  && cd jsoncpp && mkdir build && cd build \
  && cmake \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=" -stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS=" -lc++abi" \
    .. \
  && make install \
  && mkdir /usr/local/include/jsoncpp \
  && cp -r /usr/local/include/json /usr/local/include/jsoncpp/ \
  && rm -rf jsoncpp

# Asio
RUN git clone https://github.com/chriskohlhoff/asio.git \
  && cp asio/asio/include/asio.hpp /usr/local/include \
  && cp -r asio/asio/include/asio /usr/local/include \
  && rm -rf asio

# Boost.Hana (workaround branch)
RUN git clone -b bugfix/constexpr_arrays https://github.com/ricejasonf/hana.git \
  && cp -r hana/include/* /usr/local/include \
  && rm -rf hana

ARG BUILD_TYPE=Debug
ENV BUILD_TYPE ${BUILD_TYPE}

WORKDIR /opt/build
CMD cmake \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DCMAKE_CXX_FLAGS=" -stdlib=libc++ -DASIO_STANDALONE -DASIO_NO_TYPEID" \
  -DCMAKE_EXE_LINKER_FLAGS=" -lc++abi" \
  /opt/src \
  && echo '. /usr/share/bash-completion/bash_completion && set -o vi' >> /root/.bashrc \
  && /bin/bash
