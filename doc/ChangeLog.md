APBS 1.4.2 CHANGELOG
====================

These are notes for APBS version 1.4.2.
---------------------------------------

* Binary releases may be found on
 [GitHub](https://github.com/Electrostatics/apbs-pdb2pqr/releases) and
 on [SourceForge](http://sourceforge.net/projects/apbs/files/apbs/).

###New Features
* Poisson-Boltzmann Semi-Analytical Method (PB-SAM) packaged and built with APBS
* New Geometric flow API and improvements in speed (#235)
* Support for BinaryDX file format (#216)
* SOR solver added for mg-auto input file option
* DXMath improvements (#168, #216)
* Test suite improvements
  * APBS build in Travis-CI
  * Geometric Flow tests added
  * Protein RNA tests enabled (#149)
  * Intermetiate result testing (#64)
* Example READMEs onverted to markdown and updated with latest results
  

###Bug Fixes
* Geometric Flow boundary indexing bug fixed
* Build fixes:
  * Out of source CMake builds are again working
  * python library build fixes
  * Libc builds again for centos
  * Pull requests merged
* Removed irrelevant warning messages (#378)

##Notes
The following packages are treated as submodules in APBS:
* Geometric Flow has been moved to it's own [repository](https://github.com/Electrostatics/geoflow_c).
* FETk has been [cloned](https://github.com/Electrostatics/FETK) so that we have could effect updates.
* PB-SAM lives [here](https://github.com/Electrostatics/PB-SAM).
Added chat feature for users:
* https://gitter.im/Electrostatics/help

###Known Bugs
* Travis CI Linux builds are breaking because Geometric Flow relies on C++11 and Travis boxen have an old GCC that doth not support C++11.
* BEM is not building currently
