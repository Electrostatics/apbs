# APBS Changelog

## 3.3.0.alpha

Dec ??, 2021

### New Features

### Known Bugs / Limitations

### Minor Updates

### Notes


## 3.2.1

Sep 16, 2021


## 3.0.0

Jul 03, 2020

### New Features

* Poisson-Boltzmann Analytical Method (PBAM, see [Lotan & Head-Gordon](http://pubs.acs.org/doi/full/10.1021/ct050263p)) and Semi-Analytical Method (PBSAM, see [Yap & Head-Gordon](http://pubs.acs.org/doi/abs/10.1021/ct100145f)) integrated with APBS. PBSAM is currently only available in the Linux and OS X distributions.
    - Examples are located with the APBS examples in the pbam/ and pbsam/ directories.
    - More information and documentation may be found in the [PBAM](http://www.poissonboltzmann.org/external_contributions/extern-pbam/) and [PBSAM](http://www.poissonboltzmann.org/external_contributions/extern-pbsam/) sections of the APBS-PDB2PQR website.
* Tree-Code Accelerated Boundary Integral Poisson-Boltzmann Method (TABI-PB) integrated with APBS.(See [Geng & Krasny](http://www.sciencedirect.com/science/article/pii/S0021999113002404))
    - Examples are located with the APBS examples in the bem/, bem-pKa/, and bem-binding-energies/ folders
    - Included NanoShaper alternative to MSMS.
    - More information and documentation may be found in the [Contributions](http://www.poissonboltzmann.org/external_contributions/extern-tabi/) section of the APBS-PDB2PQR website
* Added binary DX format support to the appropriate APBS tools.
* Test suite amended and expanded.
* Removed hard-coded limitation to number of grid points used to determine surface accessibility.

### Known Bugs / Limitations

* PBSAM not building in windows due to C standard restrictions in the Microsoft compiler implementation.

### Minor Updates

* PB(S)AM now requires the key work 'pos' for the term argument.
* PB(S)AM 'surf' keyword has been replaced with the 'usemesh' keyword.
* PB(S)AM 'salt' keyword has been replaced with the 'ion' keyword.
* PB(S)AM dynamics parameters are no longer accepted in the ELEC section.
* PB(S)AM now has only one type of ELEC method: pb(s)am_auto.
* PB(S)AM 'gridpts' keyword has been replaced with 'dime' keyword.
* PB(S)AM 'dx' and '3dmap' keywords are deprecated to use the 'write' one instead.
* BEM mesh keyword now requires method names instead of just integer values.
* GEOFLOW ELEC type has been change from 'geoflow-auto' to 'geoflow'.
* Fixed miscellaneous Windows build issues.
* Update the build configurations for the Pythons libraries.

### Notes

* The following are included in APBS as Git submodules:
- Geometric Flow ([link](https://github.com/Electrostatics/geoflow_c/tree/e8ce510a670e0b7f3501e72be6141fc20328f947))
- FETk ([link](https://github.com/Electrostatics/FETK/tree/0c6fdeabe8929acea7481cb1480b5706b343b7e0))
- PBAM/PBSAM ([link](https://github.com/davas301/pb_solvers/tree/4805cbec02b30e9bae927f03ac2fecd3217c4dad))
- TABI-PB ([link](https://github.com/lwwilson1/TABIPB/tree/941eff91acd4153a06764e34d29b633c6e3b980f))


## 1.5.0

Oct 17, 2016

### New Features
- Poisson-Boltzmann Analytical Method (PBAM, see [Lotan & Head-Gordon](http://pubs.acs.org/doi/full/10.1021/ct050263p)) and Semi-Analytical Method (PBSAM, see [Yap & Head-Gordon](http://pubs.acs.org/doi/abs/10.1021/ct100145f)) integrated with APBS. PBSAM is currently only available in the Linux and OS X distributions.
  - Examples are located with the APBS examples in the pbam/ and pbsam/ directories.
  - More information and documentation may be found in the [PBAM](http://www.poissonboltzmann.org/external_contributions/extern-pbam/) and [PBSAM](http://www.poissonboltzmann.org/external_contributions/extern-pbsam/) sections of the APBS-PDB2PQR website.
- Tree-Code Accelerated Boundary Integral Poisson-Boltzmann Method (TABI-PB) integrated with APBS.(See [Geng & Krasny](http://www.sciencedirect.com/science/article/pii/S0021999113002404))
  - Examples are located with the APBS examples in the bem/, bem-pKa/, and bem-binding-energies/ folders
  - Included NanoShaper alternative to MSMS.
  - More information and documentation may be found in the [Contributions](http://www.poissonboltzmann.org/external_contributions/extern-tabi/) section of the APBS-PDB2PQR website
- Added binary DX format support to the appropriate APBS tools.
- Test suite amended and expanded.
- Removed hard-coded limitation to number of grid points used to determine surface accessibility.

### Known Bugs / Limitations
- PBSAM not building in windows due to C standard restrictions in the Microsoft compiler implementation.

### Bug Fixes
- Build an iAPBS build bug.
- Fixed miscellaneous Windows build issues.
- OS X bundle problem on El Capitan.

### Notes
- The following are included in APBS as Git submodules:
- Geometric Flow ([link](https://github.com/Electrostatics/geoflow_c/tree/e8ce510a670e0b7f3501e72be6141fc20328f947))
- FETk ([link](https://github.com/Electrostatics/FETK/tree/0c6fdeabe8929acea7481cb1480b5706b343b7e0))
- PBAM/PBSAM ([link](https://github.com/davas301/pb_solvers/tree/4805cbec02b30e9bae927f03ac2fecd3217c4dad))
- TABI-PB ([link](https://github.com/lwwilson1/TABIPB/tree/941eff91acd4153a06764e34d29b633c6e3b980f))
- To build on Windows from source, the root CMakeLists.txt file needs to be modified by un-commenting out the lines after "#if building on Windows" and commenting out the corresponding lines above.


## 1.4.2.1

Jan 15, 2016

### Changes from 1.4.2
- Actually included PB-AM binary, examples and documentation -- note that this is Linux and OS X only!
- Fixed Windows build so that it is not a Debug build, and ensured that no DLLs are missing

### New Features
- Poisson-Boltzmann Semi-Analytical Method (PB-AM) packaged and built with APBS
  - the binary is called `mpe` and colocated with the apbs binary
  - documentation is with the APBS documentation, and called PBE_Manual_V1.docx
  - examples are located with APBS examples in a pb-am directory
- New Geometric flow API and improvements in speed (#235)
- Support for BinaryDX file format (#216)
- SOR solver added for mg-auto input file option
- DXMath improvements (#168, #216)
- Test suite improvements
  - APBS build in Travis-CI
  - Geometric Flow tests added
  - Protein RNA tests enabled (#149)
  - Intermetiate result testing (#64)
- Example READMEs onverted to markdown and updated with latest results

### Bug Fixes
- OpenMPI (mg-para) functionality restored (#190)
- Fixed parsing PQR files that contained records other than _ATOM_ and _HETATM_ (#77, #214)
- Geometric Flow boundary indexing bug fixed
- Build fixes:
  - Out of source CMake builds are again working
  - Python library may be built (#372)
  - CentOS 5 binary builds for glibc compatibility
  - Pull requests merged
- Removed irrelevant warning messages (#378)

### Notes

The following packages are treated as submodules in APBS:
- Geometric Flow has been moved to it's own [repository](https://github.com/Electrostatics/geoflow_c)
- FETk has been [cloned](https://github.com/Electrostatics/FETK) so that we have could effect updates
- PB-SAM lives [here](https://github.com/Electrostatics/PB-SAM)

Added [chat feature](https://gitter.im/Electrostatics/help) for users.  This can also be found from the support tab on http://www.poissonboltzmann.org/.

### Known Bugs
- Travis CI Linux builds are breaking because Geometric Flow relies on C++11 and Travis boxen have an old GCC that doth not support C++11.  This is also an issue for CentOS 5
- BEM is temprarily disabled due to build issues
- Geometric Flow build is currently broken on Windows using Visual Studio


## 1.4.2

Jan 6, 2015

### New Features
- Poisson-Boltzmann Semi-Analytical Method (PB-SAM) packaged and built with APBS
- New Geometric flow API and improvements in speed (#235)
- Support for BinaryDX file format (#216)
- SOR solver added for mg-auto input file option
- DXMath improvements (#168, #216)
- Test suite improvements
  - APBS build in Travis-CI
  - Geometric Flow tests added
  - Protein RNA tests enabled (#149)
  - Intermetiate result testing (#64)
- Example READMEs onverted to markdown and updated with latest results

### Bug Fixes
- OpenMPI (mg-para) functionality restored (#190)
- Fixed parsing PQR files that contained records other than _ATOM_ and _HETATM_ (#77, #214)
- Geometric Flow boundary indexing bug fixed
- Build fixes:
  - Out of source CMake builds are again working
  - Python library may be built (#372)
  - CentOS 5 binary builds for glibc compatibility
  - Pull requests merged
- Removed irrelevant warning messages (#378)

### Notes

The following packages are treated as submodules in APBS:
- Geometric Flow has been moved to it's own [repository](https://github.com/Electrostatics/geoflow_c)
- FETk has been [cloned](https://github.com/Electrostatics/FETK) so that we have could effect updates
- PB-SAM lives [here](https://github.com/Electrostatics/PB-SAM)

Added [chat feature](https://gitter.im/Electrostatics/help) for users.  This can also be found from the support tab on http://www.poissonboltzmann.org/.

### Known Bugs
- Travis CI Linux builds are breaking because Geometric Flow relies on C++11 and Travis boxen have an old GCC that doth not support C++11.  This is also an issue for CentOS 5
- BEM is temprarily disabled due to build issues
- Geometric Flow build is currently broken on Windows using Visual Studio
