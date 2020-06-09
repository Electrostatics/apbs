#!/bin/bash

#export CC=clang
#export CXX=clang
#export CMAKE_C_COMPILER=clang
#export CMAKE_CXX_COMPILER=clang
#export CMAKE_C_LINK_EXECUTABLE=clang
#export CMAKE_CXX_LINK_EXECUTABLE=clang
export BUILD_DIR=build
export INSTALL_DIR=$HOME/apbs
export RELEASE_TYPE=Release
rm -rf $INSTALL_DIR                             || exit 1
mkdir -p $INSTALL_DIR                           || exit 1
rm -rf $BUILD_DIR                               || exit 1
mkdir $BUILD_DIR                                || exit 1
cd $BUILD_DIR                                   || exit 1
#cmake -S .. -B $BUILD_DIR --trace-source=../CMakeLists.txt --trace-expand \
cmake                                           \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR       \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE          \
      -DENABLE_GEOFLOW=ON                       \
      -DENABLE_BEM=ON                           \
      -DENABLE_FETK=ON                          \
      -DENABLE_OPENMP=ON                        \
      -DENABLE_PBAM=ON                          \
      -DENABLE_PBSAM=ON                         \
      -DENABLE_PYTHON=ON                        \
      -DENABLE_TESTS=ON                         \
      -DBUILD_SHARED_LIBS=ON                    \
      -DBUILD_DOC=OFF                           \
      ..                                        || exit 1
VERBOSE=1 make -j 1                             || exit 1
#VERBOSE=1 make -j 1 install                     || exit 1
#      -DCMAKE_C_FLAGS="-fPIC"             \
ctest -C Release --output-on-failure #           || exit 1

cd ../$BUILD_DIR
cpack -C Release -G ZIP                         || exit 
unzip -l APBS*.zip