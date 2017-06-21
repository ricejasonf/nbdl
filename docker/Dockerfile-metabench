FROM ricejasonf/emscripten
WORKDIR /usr/local/src

RUN apt-get update \
  && apt-get install -yq ruby vim \
  && npm install http-server -g

# Metabench
RUN curl -o \
  /opt/metabench.cmake \
  https://raw.githubusercontent.com/ldionne/metabench/master/metabench.cmake

# Boost.Hana (workaround branch)
RUN git clone -b bugfix/constexpr_arrays https://github.com/ricejasonf/hana.git \
  && cp -r hana/include/* /usr/local/include \
  && rm -rf hana

# Kvasir.Mpl
RUN git clone -b development https://github.com/kvasir-io/mpl.git \
  && cd mpl && git checkout 41c85408d7777b6d476fe70cd6cfcaad8f02a19c && cd .. \
  && cp -r mpl/src/* /usr/local/include \
  && rm -rf mpl

ARG BUILD_TYPE=Release
ENV BUILD_TYPE ${BUILD_TYPE}

ARG PORT=8080
ENV PORT ${PORT}
EXPOSE ${PORT}

WORKDIR /opt/build
CMD cmake \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DCMAKE_CXX_FLAGS=" -stdlib=libc++" \
  -DCMAKE_EXE_LINKER_FLAGS=" -lc++abi" \
  /opt/src \
  && echo '. /usr/share/bash-completion/bash_completion && set -o vi' >> /root/.bashrc \
  && /bin/bash
