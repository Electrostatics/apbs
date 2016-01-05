Dearest APBS users,

We are pleased to announce the latest release of APBS 1.4.2.  This latest
version of APBS includes several notable features and bug fixures.  This
release includes a major rewrite of the geometric flow code for
easier modification and testing. We are also packaging the code to run
the Poisson-Boltzmann Semi-Analytical Method (Yap et al.) with APBS in an
externals directory.  We have made major modification to improve the
build system to create out of source builds and we have incorporated 
Travis CI to test and build APBS on a nightly basis.  Our bug fixes
include changes to CMake to work more smoothly on Windows and CentOS as
well as modifications to the geometric flow code to remove overlooked
errors
from the Fortran to C conversion.

A full change log can be found
[here](https://github.com/Electrostatics/apbs-pdb2pqr/blob/master/apbs/doc/ChangeLog.md).

We thank you for your continued support of APBS.

Sincerly,

The APBS Development Team
