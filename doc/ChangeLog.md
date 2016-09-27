APBS 1.5 CHANGELOG
==================

These are notes for APBS version 1.5
------------------------------------

* Binary releases may be found on [GitHub](https://github.com/Electrostatics/apbs-pdb2pqr/releases) and on [SourceForge](http://sourceforge.net/projects/apbs/files/apbs).

### New Features

* Poisson-Boltzmann Anlytical Method (PBAM) and Semi-Anlytical Method (PBSAM) packaged and built with APBS.
    * Examples are located with the APBS examples in the pbam/ and pbsam/ directories.
    * Documentation is within the APBS documentations and more information can be found in the "Contributions" section of the [APBS-PDB2PQR](http://www.poissonboltzmann.org/) website.
* Tree-Code Accelerated Boundary Integral Poisson-Boltzmann Method (TABI-PB) packaged and built with APBS.
    * Examples are located with the APBS examples in the bem/, bem-pKa/, and bem-binding-energies/ folders.
    * Included NanoShaper alternative to MSMS (#374).
    * Documentation is within the APBS documentations and more informations can be found in the "Contributions" section fo the [APBS-PDB2PQR](http://www.poissonboltzmann.org/) website.
* Added binary DX format support to the Tools (#323).
* Test suite expanded to account for new methods.

### Bug Fixes

* Fix a bug where Geoflow and TABI-PB could not be built together.
* Build a bug when building iAPBS where it looked like an error was occuring when invoking Fortran where it was not.
* Fixed miscelaneous Windows build issues.

### Notes

The following are treated as submodules in APBS:
* Geometric Flow ([link](https://github.com/Electrostatics/geoflow_c/tree/e8ce510a670e0b7f3501e72be6141fc20328f947))
* FETk ([link](https://github.com/Electrostatics/FETK/tree/0c6fdeabe8929acea7481cb1480b5706b343b7e0))
* PBAM/PBASAM ([link](https://github.com/davas301/pb_solvers/tree/4805cbec02b30e9bae927f03ac2fecd3217c4dad))
* TABI-PB ([link](https://github.com/lwwilson1/TABIPB/tree/941eff91acd4153a06764e34d29b633c6e3b980f))