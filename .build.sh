#!/bin/bash

#export CC=clang
#export CXX=clang
#export CMAKE_C_COMPILER=clang
#export CMAKE_CXX_COMPILER=clang
#export CMAKE_C_LINK_EXECUTABLE=clang
#export CMAKE_CXX_LINK_EXECUTABLE=clang
export INSTALL_DIR=$HOME/apbs
rm -rf $INSTALL_DIR                             || exit 1
mkdir -p $INSTALL_DIR                           || exit 1
rm -rf build                                    || exit 1
mkdir build                                     || exit 1
cd build                                        || exit 1
#cmake -S .. -B build --trace-source=../CMakeLists.txt --trace-expand \
#      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
cmake                                     \
      -DCMAKE_BUILD_TYPE=Release          \
      -DENABLE_GEOFLOW=ON                 \
      -DENABLE_BEM=ON                     \
      -DENABLE_FETK=ON                    \
      -DENABLE_OPENMP=ON                  \
      -DENABLE_PBAM=ON                    \
      -DENABLE_PBSAM=ON                   \
      -DENABLE_PYTHON=ON                  \
      -DENABLE_TESTS=ON                   \
      -DBUILD_SHARED_LIBS=ON              \
      -DBUILD_DOC=OFF                     \
      ..                                        || exit 1
VERBOSE=1 make -j 1                             || exit 1
#VERBOSE=1 make -j 1 install                     || exit 1
#      -DCMAKE_C_FLAGS="-fPIC"             \
ctest  #                                         || exit 1
#cd ../tests                                     || exit 1
#TESTNAMES="actin-dimer-auto actin-dimer-parallel alkanes born FKBP geoflow hca-bind ion-pmf ion-protein ionize pka-lig point-pmf solv protein-rna"
#TESTNAMES="actin-dimer-auto"
##export LD_LIBRARY_PATH=${INSTALL_DIR}/lib
#ldd ${INSTALL_DIR}/bin/apbs
#for testname in `echo $TESTNAMES`
#do
#  pushd .
#  echo bash run_travis_test.sh $INSTALL_DIR/bin $testname
#       bash run_travis_test.sh $INSTALL_DIR/bin $testname  || exit 1
#  popd
#done

cpack -C Release    || exit 
unzip -l APBS*.zip