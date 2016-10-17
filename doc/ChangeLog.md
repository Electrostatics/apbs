APBS 1.5 CHANGELOG
==================

These are notes for APBS version 1.5
------------------------------------

* Binary releases may be found on [GitHub](https://github.com/Electrostatics/apbs-pdb2pqr/releases) and on [SourceForge](http://sourceforge.net/projects/apbs/files/apbs).

### New Features

* Poisson-Boltzmann Analytical Method (PBAM, see [Lotan & Head-Gordon](http://pubs.acs.org/doi/full/10.1021/ct050263p)) and Semi-Analytical Method (PBSAM, see [Yap & Head-Gordon](http://pubs.acs.org/doi/abs/10.1021/ct100145f)) integrated with APBS. PBSAM is currently only available in the Linux and OS X distributions.
    - Examples are located with the APBS examples in the pbam/ and pbsam/ directories.
    - More information and documentation may be found in the [PBAM](http://www.poissonboltzmann.org/external_contributions/extern-pbam/) and [PBSAM](http://www.poissonboltzmann.org/external_contributions/extern-pbsam/) sections of the APBS-PDB2PQR website.
* Tree-Code Accelerated Boundary Integral Poisson-Boltzmann Method (TABI-PB) integrated with APBS.(See [Geng & Krasny](http://www.sciencedirect.com/science/article/pii/S0021999113002404) and )
    - Examples are located with the APBS examples in the bem/, bem-pKa/, and bem-binding-energies/ folders
    - Included NanoShaper alternative to MSMS.
    - More information and documentation may be found in the [Contributions](http://www.poissonboltzmann.org/external_contributions/extern-tabi/) section of the APBS-PDB2PQR website
* Added binary DX format support to the appropriate APBS tools.
* Test suite amended and expanded.
* Removed hard-coded limitation to number of grid points used to determine surface accessibility.

### Known Bugs / Limitations

* PBSAM not building in windows due to C standard restrictions in the Microsoft compiler implementation.

### Bug Fixes

* Build an iAPBS build bug.
* Fixed miscellaneous Windows build issues.
* OS X bundle problem on El Capitan.

### Notes

* The following are included in APBS as Git submodules:
- Geometric Flow ([link](https://github.com/Electrostatics/geoflow_c/tree/e8ce510a670e0b7f3501e72be6141fc20328f947))
- FETk ([link](https://github.com/Electrostatics/FETK/tree/0c6fdeabe8929acea7481cb1480b5706b343b7e0))
- PBAM/PBSAM ([link](https://github.com/davas301/pb_solvers/tree/4805cbec02b30e9bae927f03ac2fecd3217c4dad))
- TABI-PB ([link](https://github.com/lwwilson1/TABIPB/tree/941eff91acd4153a06764e34d29b633c6e3b980f))
* To build on Windows from source, the root CMakeLists.txt file needs to be modified by un-commenting out the lines after "if building on Windows" and commenting out the corresponding lines above.
