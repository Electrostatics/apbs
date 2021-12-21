FROM ubuntu:20.04 AS apbs_base

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        software-properties-common \
        build-essential \
        dpkg-dev \
        unzip \
        cmake \
        git \
        libarpack2-dev \
        libf2c2-dev \
        libeigen3-dev \
        libboost-dev \
        python3-dev \
        libopenblas-serial-dev \
        liblapack-dev \
        libsuitesparse-dev \
        && \
    /bin/true


#########################################

FROM apbs_base

ADD . /tmp_source

ARG BLA_VENDOR="OpenBLAS"
ARG BUILD_DOC=ON
ARG BUILD_SHARED_LIBS=OFF
ARG BUILD_TOOLS=ON
ARG INSTALL_DIR=/usr/local
ARG RELEASE_TYPE=Debug
ARG ENABLE_PYGBE=ON
ARG ENABLE_BEM=ON
ARG ENABLE_GEOFLOW=ON
ARG ENABLE_FETK=ON
#ARG FETK_VERSION="1.8.1"
ARG FETK_VERSION=3bd52ae85727cdc5746397eaacd20fd0c4fc9532
ARG ENABLE_iAPBS=ON
ARG ENABLE_OPENMP=OFF
ARG ENABLE_PBAM=ON
ARG ENABLE_PBSAM=ON
ARG ENABLE_PYTHON=OFF
ARG ENABLE_TESTS=ON
ARG GET_NanoShaper=ON

RUN cd /tmp_source && \
    mkdir build && cd build && \
    cmake \
      -DCMAKE_INSTALL_INCLUDEDIR="include" \
      -DBUILD_DOC=${BUILD_DOC} \
      -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}  \
      -DBUILD_TOOLS=O${BUILD_TOOLS} \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      -DENABLE_PYGBE=${ENABLE_PYGBE} \
      -DENABLE_BEM=${ENABLE_BEM} \
      -DENABLE_iAPBS=${ENABLE_iAPBS} \
      -DENABLE_GEOFLOW=${ENABLE_GEOFLOW} \
      -DENABLE_FETK=${ENABLE_FETK}  \
      -DENABLE_OPENMP=${ENABLE_OPENMP} \
      -DENABLE_PBAM=${ENABLE_PBAM} \
      -DENABLE_PBSAM=${ENABLE_PBSAM} \
      -DENABLE_PYTHON=${ENABLE_PYTHON} \
      -DENABLE_TESTS=${ENABLE_TESTS} \
      -DFETK_VERSION=${FETK_VERSION} \
      -DGET_NanoShaper=${GET_NanoShaper} \
      .. && \
    make -j install && \
    /bin/true

#RUN cd /tmp_source && \
#    ./.build.sh && \
#    /bin/true
