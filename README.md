apbs
============

![Build Status](https://travis-ci.org/Electrostatics/apbs-pdb2pqr.svg?branch=master)

To build the latest branch, follow the instructions below...

# Import submodules:
We are using Git submodules to manage various pieces of code.  To build the master branch, after cloning it, you will need to do the following from within the root `apbs-pdb2pqr` directory:
 * `git submodule init`
 * `git submodule update`

# Build flags for CMake
* `cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOC=OFF -DBUILD_SHARED_LIBS=OFF <apbs src location>`

## Using geometric flow
If you want to use the geometric flow implementation, when invoking CMake, set ENABLE_GEOFLOW to ON, e.g., `-DENABLE_GEOFLOW=ON`.

## Adding finite element support (fe-manual)
If you would like to use the FEM, you will need to be on OS X or Linux.  To enable, when invoking CMake, set ENABLE_FETK to ON, e.g., `-DENABLE_FETK=ON`.
On Linux the FETK shared libraries need to be locatable by the shared library loader.  One way to do this is to update LD_LIBRARY_PATH to point at `<build-dir>/fetk/lib`, e.g., `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<build-dir>/fetk/lib`.

## Building the python libraries for pdb2pqr
* Install [swig](http://www.swig.org/)
* Build APBS with the following flag: `-DENABLE_PYTHON=ON`.  If you are on linux you also need `-DBUILD_SHARED_LIBS=OFF`
* Add these libraries to your library path so pdb2pqr can find them.
