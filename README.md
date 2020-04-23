apbs
============

![Build Status](https://travis-ci.org/Electrostatics/apbs-pdb2pqr.svg?branch=master)

To build the latest branch, follow the instructions below...

# Import submodules:
We are using Git submodules to manage various pieces of code.  To build the master branch, after cloning it, you will need to do the following from within the root `apbs-pdb2pqr` directory:
 * `git submodule init`
 * `git submodule update`

# Build flags for CMake
* `cmake <path to apbs directory>`

## Using geometric flow
If you want to use the geometric flow implementation, when invoking CMake, set ENABLE_GEOFLOW to ON, e.g., `-DENABLE_GEOFLOW=ON`.

## Using PB-AM
If you want to use the Poisson-Boltzmann Analytical Method developed by the Teresa Head-Gordon lab, when invoking CMake, set ENABLE_PBAM to ON, e.g., `-DENABLE_PBAM=ON`. PB-AM currently runs on OS X or Linux only.

## Using TABI-PB
If you want to use the Treecode-Accelerated Boundary Integral method (TABI-PB) developed by Robert Krasny and Weihua Geng, when invoking CMake, set ENABLE_BEM to ON, e.g., `-DENABLE_BEM=ON`. TABIPB builds and runs on OS X, Linux, and Windows.

TABI-PB requires the use of a molecular surface mesh generation software to create a surface representation of the molecule. By default, TABI-PB uses MSMS to generate a solvent excluded surface (SES), but it also supports the use of NanoShaper to generate an SES or Skin surface. See TABI-PB documentation for details on choosing NanoShaper. When TABI-PB runs, it will attempt to generate a surface mesh by looking in your path for the mesh generation executable. A user can obtain the appropriate executable using the steps described below. The user then must place these executables in their path.

## Getting MSMS and NanoShaper executables
MSMS, developed by Michel Sanner, and NanoShaper, developed by W. Rocchia and S. Decherchi, are molecular surface mesh generation software. If you want an executable of MSMS or NanoShaper already built for your system, when invoking CMake, set GET_MSMS to ON, e.g., `-DGET_MSMS=ON`, or GET_NanoShaper to ON, e.g., `-DGET_NanoShaper=ON`, respectively. The executables will be placed in the bin of your build. Executables are current pre-built for OS X, Linux, and Windows.

## Adding finite element support (fe-manual)
If you would like to use the FEM, you will need to be on OS X or Linux.  To enable, when invoking CMake, set ENABLE_FETK to ON, e.g., `-DENABLE_FETK=ON`.
On Linux the FETK shared libraries need to be locatable by the shared library loader.  One way to do this is to update LD_LIBRARY_PATH to point at `<build-dir>/fetk/lib`, e.g., `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<build-dir>/fetk/lib`.

## Building the APBS python libraries (needed for pdb2pqr)
* Install [swig](http://www.swig.org/)
* Build APBS with the following flag: `-DENABLE_PYTHON=ON`.  If you are on Linux you also need `-DBUILD_SHARED_LIBS=OFF`
* Add these libraries to your library path so pdb2pqr can find them.
