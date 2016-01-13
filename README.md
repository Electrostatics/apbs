apbs
============

![Build Status](https://travis-ci.org/Electrostatics/apbs-pdb2pqr.svg?branch=master)

# Latest release

If you are looking for the source for the latest release of APBS, be sure to pull the 1.4.1 release branch:

`git checkout 1.4.1-binary-release`

# Current development (master) branch
We are using Git submodules to manage various pieces of code.  To build the master branch, after cloning it, you will need to do the following from within the root `apbs-pdb2pqr` directory:
 * `git submodule init`
 * `git submodule update`

## Geometric Flow
If you want to use the geometric flow implementation, when invoking CMake, set ENABLE_GEOFLOW to ON, e.g., `-DENABLE_GEOFLOW=ON`.

## Finite Element support (fe-manual)
If you would like to use the FEM, you will need to be on OS X or Linux.  To enable, when invoking CMake, set ENABLE_FETK to ON, e.g., `-DENABLE_FETK=ON`.
On Linux the FETK shared libraries need to be locatable by the shared library loader.  One way to do this is to update LD_LIBRARY_PATH to point at `<build-dir>/fetk/lib`, e.g., `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<build-dir>/fetk/lib`.

## Building the python libraries for pdb2pqr
Use `cmake <path to apbs> -DENABLE_PYTHON=ON -DBUILD_SHARED_LIBS=ON`.  Be sure to add these libraries to your library path so pdb2pqr can find them.
