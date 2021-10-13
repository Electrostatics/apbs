FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        build-essential \
        dpkg-dev \
        unzip \
        cmake \
        git \
        libarpack2-dev \
        libopenblas-dev \
        liblapack-dev \
        libsuperlu-dev \
        libf2c2-dev \
        libsuitesparse-dev \
        libeigen3-dev \
        libboost-dev \
        && \
    /bin/true

ADD . /tmp_source

ARG INSTALL_DIR=/usr/local
ARG RELEASE_TYPE=Debug
ARG ENABLE_PYGBE=OFF
ARG ENABLE_BEM=ON
ARG ENABLE_GEOFLOW=ON
ARG ENABLE_FETK=ON
ARG ENABLE_PBAM=ON
ARG ENABLE_PBSAM=ON
ARG FETK_VERSION=1.8.1

RUN cd /tmp_source && \
    mkdir build && cd build && \
    cmake \
      -DCMAKE_INSTALL_INCLUDEDIR="include" \
      -DBUILD_DOC=ON \
      -DBUILD_SHARED_LIBS=ON  \
      -DBUILD_TOOLS=ON \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      -DENABLE_PYGBE=${ENABLE_PYGBE} \
      -DENABLE_BEM=${ENABLE_BEM} \
      -DENABLE_GEOFLOW=${ENABLE_GEOFLOW} \
      -DENABLE_FETK=${ENABLE_FETK}  \
      -DENABLE_OPENMP=ON \
      -DENABLE_PBAM=${ENABLE_PBAM} \
      -DENABLE_PBSAM=${ENABLE_PBSAM} \
      -DENABLE_PYTHON=OFF \
      -DENABLE_TESTS=ON \
      -DENABLE_TINKER=OFF \
      -DFETK_VERSION=${FETK_VERSION} \
      -DGIT_SUBMODULE=OFF \
      .. && \
    make -j install && \
    /bin/true

#RUN cd /tmp_source && \
#    ./.build.sh && \
#    /bin/true
