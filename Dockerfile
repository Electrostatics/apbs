FROM ubuntu:20.04 AS apbs_base

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        software-properties-common \
        build-essential \
        dpkg-dev \
        unzip \
        cmake \
        git \
        wget \
        libarpack2-dev \
        libf2c2-dev \
        libeigen3-dev \
        libboost-dev \
        python3-dev \
        python3-pip \
        libopenblas-serial-dev \
        liblapack-dev \
        libsuitesparse-dev \
        && \
    wget http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz && \
    gunzip metis-5.1.0.tar.gz && \
    tar -xf metis-5.1.0.tar && \
    cd metis-5.1.0 && \
    make config prefix=/usr/local && \
    make install && \
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
ARG FETK_VERSION="1.9.0"
#ARG FETK_VERSION=af45142e23bc98371f797e8a69a9308b032228c9
ARG ENABLE_iAPBS=ON
ARG ENABLE_OPENMP=OFF
ARG ENABLE_PBAM=OFF
ARG ENABLE_PBSAM=OFF
ARG ENABLE_PYTHON=OFF
ARG ENABLE_TESTS=ON
ARG GET_NanoShaper=ON
ARG PYTHON_MIN_VERSION="3.9"
ARG PYTHON_MAX_VERSION="3.10"

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
      -DPYTHON_MIN_VERSION="${PYTHON_MIN_VERSION}" \
      -DPYTHON_MAX_VERSION="${PYTHON_MAX_VERSION}" \
      .. && \
    make -j install && \
    /bin/true

#RUN cd /tmp_source && \
#    ./.build.sh && \
#    /bin/true
