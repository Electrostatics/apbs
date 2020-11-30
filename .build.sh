#!/bin/bash

ostype="$(uname -s)"
case "${ostype}" in
      # Darwin*)    export CC=clang; export CXX=clang++;;
      Darwin*)    export CC=gcc-9; export CXX=g++-9;;
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
echo "==================================== Install Python requirements ==================================== "
pip3 install -U pip
pip3 install -U pytest
pip3 install -U virtualenv
pip3 install -U numpy
pip3 install -r requirements.txt
#  Just build APBS for now
echo "==================================== PWD FOR TOP DIR ==================================== "
pwd
echo "==================================== Get External SubModules ==================================== "
git submodule init
git submodule update
git submodule sync

echo "==================================== CLEAN =============================================== "
rm -rf $BUILD_DIR                                         || exit 1
rm -rf $INSTALL_DIR                                       || exit 1
mkdir -p $BUILD_DIR                                       || exit 1
mkdir -p $INSTALL_DIR                                     || exit 1
#  Build pybind11
export BUILD_PYBIND=0
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
#cmake -S .. -B $BUILD_DIR --trace-source=../CMakeLists.txt --trace-expand \
cmake                                                     \
      -DBUILD_DOC=ON                                      \
      -DBUILD_SHARED_LIBS=OFF                             \
      -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} ${COVERAGE}"      \
      -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} ${COVERAGE}"  \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE                    \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR                 \
      -DENABLE_BEM=ON                                     \
      -DENABLE_GEOFLOW=ON                                 \
      -DENABLE_FETK=ON                                    \
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
# Run a single test if it fails using the following:
# ctest -VV -R pbam_test
echo "==================================== TEST =============================================== "
ctest -C Release --output-on-failure                      #|| exit 1

echo "==================================== PACKAGE ============================================ "
cpack -C Release -G ZIP                                   || exit 1
unzip -l APBS*.zip
mkdir -p $HOME/artifacts
mv APBS*.zip $HOME/artifacts