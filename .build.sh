#!/bin/bash
 
# echo "==================================== WHICH GCC ==================================== "
# which gcc
# ls /usr/local/bin/gcc*
 
# Install prereqs with INSTALL_PREREQS=1 or turn off with INSTALL_PREREQS=0
export INSTALL_PREREQS=1
 
# Setup tests for APBS with TEST_APBS=1 or turn off with TEST_APBS=0
export TEST_APBS=0
 
# Create ZIP archive for APBS with PACKAGE_APBS=1 or turn off with PACKAGE_APBS=0
export PACKAGE_APBS=0
 
# Configure and Build pybind11 with BUILD_PYBIND=1 or turn off with BUILD_PYBIND=0
export BUILD_PYBIND=0
 
echo "==================================== WHICH GCC ==================================== "
ostype="$(uname -s)"
case "${ostype}" in
      # NOTE: clang on Github Actions cannot fine Accelerate Framework
      #       so you will get errors about not being able to find xerbla_
      # Darwin*)    export CC=clang; export CXX=clang++;;
      Darwin*)    export CC=gcc-9; export CXX=g++-9; export CMAKE_C_FLAGS="-Wl,-rpath=@executable_path/../lib "; export CMAKE_CXX_FLAGS="-Wl,-rpath=@executable_path/../lib ";;
      Linux*)     export CC=gcc-9; export CXX=g++-9;;
esac
 
export CMAKE_C_COMPILER=$CC
export CMAKE_CXX_COMPILER=$CXX
export CMAKE_C_LINK_EXECUTABLE=$CC
export CMAKE_CXX_LINK_EXECUTABLE=$CXX
 
export SRC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export COVERAGE="-g -O0 -fprofile-arcs -ftest-coverage"
export COVERAGE=""
export BUILD_DIR="$SRC_DIR/build"
export INSTALL_DIR=$HOME/apbs
export PATH=$INSTALL_DIR:$PATH
export RELEASE_TYPE=Debug
export RELEASE_TYPE=Release
 
echo "==================================== WHERE AM I ==================================== "
pwd
echo "==================================== VERSIONS: ==================================== "
echo "==================================== PYTHON VERSION"
python -c "import sys; print(sys.version)"
echo "==================================== CMAKE VERSION"
cmake --version
echo "==================================== C Compiler VERSION"
$CMAKE_C_COMPILER --version
echo "==================================== C++ Compiler VERSION"
$CMAKE_CXX_COMPILER --version
echo "==================================== SWIG VERSION"
swig -version
 
if [ "${BUILD_PYBIND}" -ne "0" ]; then
    echo "==================================== Install Python requirements ==================================== "
    python -m pip install -U pip
    python -m pip install -U pytest
    python -m pip install -U virtualenv
    python -m pip install -U numpy
    python -m pip install -r requirements.txt
fi
 
#  Just build APBS for now
echo "==================================== PWD FOR TOP DIR ==================================== "
pwd
echo "==================================== Get External SubModules ==================================== "
git submodule init
git submodule update
git submodule sync
git ls-tree HEAD externals
 
echo "==================================== CLEAN =============================================== "
rm -rf $BUILD_DIR                                         || exit 1
rm -rf $INSTALL_DIR                                       || exit 1
mkdir -p $BUILD_DIR                                       || exit 1
mkdir -p $INSTALL_DIR                                     || exit 1
echo "==================================== SETUP ENV =============================================== "
export LD_LIBRARY_PATH=$HOME/apbs/lib:${LD_LIBRARY_PATH}
export PATH=$HOME/apbs/bin:${PATH}
 
if [ "${BUILD_PYBIND}" -ne "0" ]; then
    echo "==================================== PYBIND =============================================== "
    pushd $(pwd)/externals/pybind11
    [ -d build ] || mkdir -p build
    [ -d install ] || mkdir -p install
    pushd build
    cmake                              \
        -DDOWNLOAD_CATCH=ON          \
        -DPYBIND11_TEST=OFF          \
        -DCMAKE_INSTALL_PREFIX=$(python3 -c "import os,sys; print(os.path.realpath(sys.argv[1]))" ../install) \
        ..
    make -j install
    popd
    export pybind11_DIR=$(python3 -c "import os,sys; print(os.path.realpath(sys.argv[1]))" ./install)
    echo "pybind11_DIR=${pybind11_DIR}"
    popd
fi
 
echo "==================================== CONFIG =============================================== "
cd $BUILD_DIR                                             || exit 1
#cmake                                                     \
cmake -S .. -B $BUILD_DIR --trace-source=../externals/geoflow_c/src/CMakeLists.txt --trace-expand \
      -DCMAKE_INSTALL_INCLUDEDIR="include"                \
      -DBUILD_DOC=ON                                      \
      -DBUILD_SHARED_LIBS=ON                              \
      -DBUILD_TOOLS=ON                                    \
      -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} ${COVERAGE}"      \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} ${COVERAGE}"  \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE                    \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR                 \
      -DENABLE_PYGBE=ON                                   \
      -DENABLE_BEM=ON                                     \
      -DENABLE_GEOFLOW=OFF                                \
      -DENABLE_FETK=OFF                                   \
      -DENABLE_OPENMP=ON                                  \
      -DENABLE_PBAM=ON                                    \
      -DENABLE_PBSAM=ON                                   \
      -DENABLE_PYTHON=$BUILD_PYBIND                       \
      -DENABLE_TESTS=ON                                   \
      -DENABLE_TINKER=OFF                                 \
      ..                                                  || exit 1
 
echo "==================================== BUILD =============================================== "
VERBOSE=1 make -j 1                                       || exit 1
VERBOSE=1 make -j 1 install                               #|| exit 1
export PATH="$INSTALL_DIR/bin:$PATH"
 
if [ "${TEST_APBS}" -ne "0" ]; then
    # Run a single test if it fails using the following:
    # echo "==================================== VERBOSE TEST ======================================= "
    # ctest -C Release -VV -R protein-rna
    echo "==================================== TEST =============================================== "
    ctest -C Release --output-on-failure                      #|| exit 1
fi
 
if [ "${PACKAGE_APBS}" -ne "0" ]; then
    echo "==================================== PACKAGE ============================================ "
    cpack -C Release -G ZIP                                   || exit 1
    unzip -l APBS*.zip
    mkdir -p $HOME/artifacts
    mv APBS*.zip $HOME/artifacts
fi