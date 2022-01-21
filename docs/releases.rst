===============
Release history
===============


---------------------
APBS 3.4.0 (Jan 2022)
---------------------

* Binary releases may be found in `GitHub releases <https://github.com/Electrostatics/apbs/releases>`_.

^^^^^^^^^^^^
New Features
^^^^^^^^^^^^

* Revamped build system
* Most submodule switched to using CMake's FetchContent
* FETK is now required; using v1.9.1
* Automatic release processes implemented
* Cross-platform builds performed on GitHub Actions
* Pre-compiled binaries posted to each Release
* Binaries are currently single-threaded (no OpenMP)

^^^^^^^^^^^^^^^^^^^^^^^^^^
Known Bugs and Limitations
^^^^^^^^^^^^^^^^^^^^^^^^^^

* Automated build is only single threaded
* pb_solvers has been disabled; requires further development for full integration

^^^^^
Notes
^^^^^

* The following are included in APBS:

  * `Geometric Flow <https://github.com/Electrostatics/geoflow_c/tree/39d53269c084f1dc1caa71de95dca77f19da739e>`_
  * `FETk <https://github.com/Electrostatics/FETK/tree/8c2b67fe587336ba73f77573f13e31ecb1a5a7f9>`_
  * `PBAM/PBSAM <https://github.com/Electrostatics/pb_solvers/tree/d3ba994d7ec2b2cad5b3e843784c7cb9f41ace37>`_
  * `TABI-PB <https://github.com/Treecodes/TABI-PB/tree/fe1c237b057418fed48535db125394607040d9de>`_


-------------------
APBS 3.0 (May 2020)
-------------------

* Binary releases may be found in `GitHub releases <https://github.com/Electrostatics/apbs/releases>`_ and on `SourceForge <http://sourceforge.net/projects/apbs/files/apbs>`_.

^^^^^^^^^^^^
New Features
^^^^^^^^^^^^

* Poisson-Boltzmann Analytical Method (PBAM, see `Lotan & Head-Gordon <http://pubs.acs.org/doi/full/10.1021/ct050263p>`_) and Semi-Analytical Method (PBSAM, see `Yap & Head-Gordon <http://pubs.acs.org/doi/abs/10.1021/ct100145f>`_) integrated with APBS. PBSAM is currently only available in the Linux and OS X distributions.

  * Examples are located with the APBS examples in the pbam/ and pbsam/ directories.

* Tree-Code Accelerated Boundary Integral Poisson-Boltzmann Method (TABI-PB) integrated with APBS (See `Geng & Krasny <http://www.sciencedirect.com/science/article/pii/S0021999113002404>`_).

  * Examples are located with the APBS examples in the bem/, bem-pKa/, and bem-binding-energies/ folders
  * Included NanoShaper alternative to MSMS.
  * More information and documentation may be found in the `Contributions <http://www.poissonboltzmann.org/external_contributions/extern-tabi/>`_ section of the APBS website

* Added binary DX format support to the appropriate APBS tools.
* Test suite amended and expanded.
* Removed hard-coded limitation to number of grid points used to determine surface accessibility.

^^^^^^^^^^^^^^^^^^^^^^^^^^
Known Bugs and Limitations
^^^^^^^^^^^^^^^^^^^^^^^^^^

* PBSAM not building in windows due to C standard restrictions in the Microsoft compiler implementation.

^^^^^^^^^^^^^
Minor Updates
^^^^^^^^^^^^^

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

^^^^^
Notes
^^^^^

* The following are included in APBS as Git submodules:

  * `Geometric Flow <https://github.com/Electrostatics/geoflow_c/tree/e8ce510a670e0b7f3501e72be6141fc20328f947>`_
  * `FETk <https://github.com/Electrostatics/FETK/tree/0c6fdeabe8929acea7481cb1480b5706b343b7e0>`_
  * `PBAM/PBSAM <https://github.com/davas301/pb_solvers/tree/4805cbec02b30e9bae927f03ac2fecd3217c4dad>`_
  * `TABI-PB <https://github.com/lwwilson1/TABIPB/tree/941eff91acd4153a06764e34d29b633c6e3b980f>`_


-------------------
APBS 1.5 (Oct 2016)
-------------------

Dearest APBS users,

We are pleased to announce the latest release of APBS 1.5. The latest version of APBS includes several notable features and bug fixes. This release includes the addition of Poisson-Boltzmann Analytical-Method (PB-AM), Poisson-Boltzmann Semi-Analytical Method (PB-SAM) and the Treecode-Accelerated Boundary Integral Poisson-Boltzmann method (TABI). Additionally, we have made improvements to the build system and the system tests, as well as miscellaneous bug fixes. A full change log may be found `here <https://github.com/Electrostatics/apbs/blob/master/apbs/doc/ChangeLog.md>`_.

For help with installation, building, or running APBS, please visit https://gitter.im/Electrostatics/help.

We thank you for your continued support of APBS.

Sincerely,

The APBS Development Team

-----------------------
APBS 1.4.2.1 (Jan 2016)
-----------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Poisson-Boltzmann Semi-Analytical Method (PB-SAM) packaged and built with APBS.
* New Geometric flow API and improvements in speed.
* Support for BinaryDX file format.
* SOR solver added for mg-auto input file option.
* DXMath improvements.
* Test suit improvements:

  * APBS build in Travis-CI
  * Geometric Flow tests added.
  * Protein RNA tests enabled.
  * Intermediate results testing.

* Example READMEs onverted to markdown and updated with latest results. 

^^^^^^^^^
Bug fixes
^^^^^^^^^

* OpenMPI (mg-para) functionality restored.
* Fixed parsing PQR files that contained records other than ATOM and HETATM.
* Geometric Flow boundary indexing bug fixed.
* Build fixes:

  * Out of source CMake build are again working.
  * Python library may be built.
  * CentOS 5 binary builds for glibc compatibility.
  * Pull requests merged.

* Removed irrelevant warning messages.

^^^^^
Notes
^^^^^

The following packages are treated as submodules in APBS:

* Geometric Flow has been moved to its own `repository <https://github.com/Electrostatics/geoflow_c>`_.
* FETk has been `cloned <https://github.com/Electrostatics/FETK>`_ so that we could effect updates.
* PB-SAM lives here:  https://github.com/Electrostatics/PB-SAM

Added a `chat feature <https://gitter.im/Electrostatics/help>`_ for users.

^^^^^^^^^^
Known bugs
^^^^^^^^^^

* Travis CI Linux builds are breaking because Geometric Flow relies on C++11 and Travis boxen have an old GCC that does not support C++11. This also and issue for CentOS 5.
* BEM is temporarily disabled due to build issues.
* Geometric Flow build is currently broken on Windows using Visual Studio.

-----------------------
APBS 1.4.2.0 (Jan 2016)
-----------------------

^^^^^^^^^^^^^
Binary builds
^^^^^^^^^^^^^

Binary releases are available.

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Poisson-Boltzmann Semi-Analytical Method (PB-SAM) packaged and build with APBS.
* New Geometric flow API and improvements: https://github.com/Electrostatics/apbs/issues/235
* Support for BinaryDX file format: https://github.com/Electrostatics/apbs/issues/216
* SOR solver added for mg-auto input file option.
* DXMath improvements https://github.com/Electrostatics/apbs/issues/168 and https://github.com/Electrostatics/apbs/issues/216
* Test suite improvements:

  * APBS build in Travis-CI
  * Geometric Flow test added.
  * Protein RNA test enabled https://github.com/Electrostatics/apbs/issues/149
  * Intermediate result testing https://github.com/Electrostatics/apbs/issues/64

* Example READMEs converted to markdown and updated with latest results.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* OpenMPI (mg-para) functionality restored: https://github.com/Electrostatics/apbs/issues/190
* Fized parsing PQR files that contained records other than ATOM and HETATM: https://github.com/Electrostatics/apbs/issues/77 and https://github.com/Electrostatics/apbs/issues/214
* Geometrix Flow boundary indexing bug fixed.
* Build fixes:

  * Out of source CMake build are again working.
  * Python library may be built:  https://github.com/Electrostatics/apbs/issues/372
  * CentOS 5 binary builds for glibc compability.
  * Pull requests merged.

*  Removed irrelevant warning messages: https://github.com/Electrostatics/apbs/issues/378

^^^^^
Notes
^^^^^

* The following packages are treated as submodules in APBS:

  * Geometric Flow has been moved to its own repository:  https://github.com/Electrostatics/geoflow_c/
  * FETk has been cloned: https://github.com/Electrostatics/FETK/
  * PB-SAM lives here:  https://github.com/Electrostatics/PB-SAM/

* Added chat feature at https://gitter.im/Electrostatics/help/ for users. 

^^^^^^^^^^
Known bugs
^^^^^^^^^^

* Travis-CI Linux builds are breaking because Geometric Flow relies on C++11 and Travis boxen have an old GCC that does not support C++11. This is also an issue for CentOS 5.
* BEM is temporarily disabled due to build issues.
* Geometric Flow build is currently broken on Windows using Visual Studio.

---------------------
APBS 1.4.1 (Aug 2014)
---------------------

^^^^^^^
Summary
^^^^^^^

We are pleased to announced the release of APBS 1.4.1. This was primarily a bug fix release; however, we have added a few features we'd like to hightlight below.
We would like to also highlight our new website, still located at: http://www.poissonboltzmann.org. This site is also hosted at GitHub and we hope that the new organization will make it easier for people to find the content they need. While we are still in the process of migrating some remaining content, we have added links to the previous page when needed.
Thank you for your continuing support of APBS. As always, please use our mailing list to send up questions or comments about our software.

^^^^^^^^^^^^^^^^
Detailed changes
^^^^^^^^^^^^^^^^

* Multigrid bug fix for volumes with large problem domain.
* We have added a preliminary implementation of geometric flow.
* Finite element method support has been re-enabled.
* Migration of the APBS source tree to `GitHub <http://github.com/Electrostatics/apbs>`_ for better collaboration, issue tracking, and source code management.
* Improved test suite.

---------------------
APBS 1.4.0 (Jul 2012)
---------------------

^^^^^^^
Summary
^^^^^^^

We are pleased to announce the release of APBS 1.4.0. This version of APBS includes a massive rewrite to eliminate FORTRAN from the software code base to improve portability and facilitate planned optimization and parallelization activities. A more detailed list of changes is provided below.
Starting with this release, we have created separate installation packages for the APBS binaries, examples, and programming documentation. This change is in response to user requests and recognition of the large size of the examples and documentation directories.

^^^^^^^^^^^^^^^^
Detailed changes
^^^^^^^^^^^^^^^^


* Removed FORTRAN dependency from APBS
* Direct line by line translation of all source from contrib/pmgZ
* Functions replaced and tested incrementally to ensure code congruence
* Created new subfolder src/pmgC for translated pmg library
* Created new macros for 2d, 3d matrix access
* In src/generic/apbs/vmatrix.h
* Simulate native FORTRAN 2 and 3 dimensional arrays
* Use 1-indexed, column-major ordering
* Allowed direct 1-1 translation from FORTRAN to ensurre code congruence
* Added additional debugging and output macros to src/generic/apbs/vhal.h
* Added message, error message, assertion, warning, and abort macros
* Macro behavior modified by the --enable-debug flag for configure
* Non-error messages directed to stderr in debug, io.mc otherwise
* All error messages are directed to stdout
* In debug mode, verbose location information is provided
* Added additional flags to configure
* --with-fetk replaces FETK_INCLUDE, FETK_LIBRARY environment flags
* --with-efence enables compiling with electric fence library
* --enable-debug eliminates compiling optimization and includes line no info
* ---enable-profiling adds profiling information and sets --enable-debug
* --enable-verbose-debug prints lots of function specific information

-------------------
APBS 1.3 (Oct 2010)
-------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Added in new read and write binary (gz) commands. Can read gzipped DX files directly.
* Added new write format to output the atomic potentials to a flat file (see atompot)
* Added new functionality for using a previously calculated potential map for a new calculation.
* Added a new program for converting delphi potential maps to OpenDX format. tools/mesh/del2dx
* Updated Doxygen manual with call/caller graphs.  Replaced HTML with PDF.
* Added tools/matlab/solver with simple Matlab LPBE solver for prototyping, teaching, etc.
* Deprecated APBS XML output format.
* Deprecated nlev keyword.
* Added etol keyword, which allows user-defined error tolerance in LPBE and NPBE calculations (default errtol value is 1.0e-6).
* Added more explanatory error messages for the case in which parm keyword is missing from APBS input file for apolar calculations.
* Added a polar and apolor forces calculation example to examples/born/ .
* Added warning messages for users who try to compile APBS with --enable-tinker flag and run APBS stand-alone execution.
* Switched default Opal service urls from sccne.wustl.edu to NBCR.
* Added a sanity check in routines.c: 'bcfl map' in the input file requires 'usemap pot' statement in the input file as well.
* Introduced Vpmgp_size() routine to replace F77MGSZ call in vpmg.c
* Updated test results for APBS-1.3 release.
    
^^^^^^^^^
Bug fixes
^^^^^^^^^

* Modified Vpmg_dbForce with some grid checking code provided by Matteo Rotter.
* Fixed a bug in psize.py per Michael Lerner's suggestion. The old version of psize.py gives wrong cglen and fglen results in special cases (e.g., all y coordinates are negative values).
* Fixed a bug in examples/scripts/checkforces.sh: the condition for "Passed with rounding error" is abs(difference) < errortol, not the other way around.
* Fixed the help string in ApbsClient.py .
* Fixed a bug in Vacc_atomdSASA(): the atom SASA needs to be reset to zero displacement after finite melement methods.
* Fixed a bug in Vpmg_dbForce(): the initialization of rtot should appear before it is used.
* Fixed a bug in initAPOL(): center should be initialized before used.
* Fixed a bug in routines.c: eliminated spurious "Invalid data type for writing!" and "Invalid format for writing!" from outputs with "write atompot" statement in the input file.
* Fixed a bug in vpmg.c: fixed zero potential value problem on eges and corners in non-focusing calculations.

---------------------
APBS 1.2.1 (Dec 2009)
---------------------

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Added in warning into focusFillBound if there is a large value detected in setting the boundary conditions during a focusing calculation
* Added in a check and abort in Vpmg_qmEnergy if chopped values are detected. This occurs under certain conditions for NPBE calculations where focusing cuts into a low-dielectric regions.
* Fixed a bug in Vpmg_MolIon that causes npbe based calculations to return very large energies.  This occurs under certain conditions for NPBE calculations where focusing cuts into a low-dielectric regions.

---------------------
APBS 1.2.0 (Oct 2009)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Updated NBCR opal service urls from http://ws.nbcr.net/opal/... to http://ws.nbcr.net/opal2/... 
* Increased the number of allowed write statements from 10 to 20
* Updated inputgen.py with --potdx and --istrng options added, original modification code provided by Miguel Ortiz-Lombardía
* Added more information on PQR file parsing failures
* Added in support for OpenMP calculations for multiprocessor machines.
* Changed default Opal service from http://ws.nbcr.net/opal2/services/APBS_1.1.0 to http://sccne.wustl.edu:8082/opal2/services/apbs-1.2

^^^^^^^^^^^^^
Modifications
^^^^^^^^^^^^^

* Applied Robert Konecny's patch to bin/routines.h (no need to include apbscfg.h in routines.h)

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Added a remove_Valist function in Python wrapper files, to fix a memory leak problem in pdb2pka
* Fixed a bug in smooth.c: bandwidth iband, jband and kband (in grid units) should be positive integers
* Fixed a bug in psize.py: for a pqr file with no ATOM entries but only HETATM entries in it, inputgen.py should still create an APBS input file with reasonable grid lengths
* Fixed a bug in Vgrid_integrate: weight w should return to 1.0 after every i, j or k loop is finished
* Fixed a bug in routines.c, now runGB.py and main.py in tools/python/ should be working again instead of producing segfault
* Fixed a few bugs in ApbsClient.py.in related to custom-defined APBS Opal service urls, now it should be OK to use custom-defined APBS Opal service urls for PDB2PQR web server installations

---------------------
APBS 1.1.0 (Mar 2009)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Moved APBS user guide and tutorial to MediaWiki
* Added in support for OpenMPI for parallel calculations
* Added in command line support for Opal job submissions (Code by Samir Unni)
* Allowed pathname containing spaces in input file, as long as the whole pathname is in quotes ("")
* Documented 'make test' and related features

^^^^^^^^^^^^^
Modifications
^^^^^^^^^^^^^

* Modified the function bcCalc to march through the data array linearly when setting boundary conditions. This removes duplication of grid points on the edge of the array and corners.
* Clarified documentation on the IDs assigned to input maps, PQRs, parameter files, etc.
* pdated tutorial to warn against spaces in APBS working directory path in VMD; updated user guide to warn against spaces in APBS installation path on Windows
* 'make test' has been reconfigured to run before issuing make install (can be run from top directory)
* Removed tools/visualization/vmd from tools directory in lieu of built-in support in VMD
* Path lengths can now be larger than 80 characters
* Expanded authorship list
* Added in 'make test-opal' as a post install test (run from the examples install directory)
* Added additional concentrations to protein-rna test case to better encompass experimental conditions used by Garcia-Garcia and Draper; this improves agreement with the published data

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Fixed typos in User Guide (ion keyword) and clarified SMPBE keyword usage
* Fixed typo in User Guide (writemat: poission -> poisson)
* Updated psize.py with Robert's patch to fix inconsistent assignment of fine grid numbers in some (very) rare cases
* Fixed bug with boundary condition assignment.  This could potentially affect all calculations; however, probably has limited impact:  many test cases gave identical results after the bug fix; the largest change in value was < 0.07%.

---------------------
APBS 1.0.0 (Apr 2008)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^


* Changed license to New BSD style open source license (see http://www.opensource.org/licenses/bsd-license.php) for more information
* Added in a feature limited version of PMG (Aqua) that reduces the memory footprint of an APBS run by 2-fold
* Modified several routines to boost the speed of APBS calculations by approximately 10% when combined with the low memory version of APBS
* Simplified parameter input for ION and SMPBE keywords (key-value pairs) 
* Examples and documentation for size-modified PBE code (Vincent Chu et al)
* Added in "fast" compile-time option that uses optimized parameters for multigrid calculations
* mg-dummy calculations can be run with any number (n>3) of grid points
* Updated PMG license to LGPL
* Added per-atom SASA information output from APOLAR calculations
* Added per-atom verbosity to APOLAR calculation outputs
* Ability to read-in MCSF-format finite element meshes (e.g., as produced by Holst group GAMER software)
* Updated installation instructions in user guide
* Updated inputgen.py to write out the electrostatic potential for APBS input file.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Updated tools/python/apbslib* for new NOsh functionality
* Clarified ELEC/DIME and ELEC/PDIME documentation
* Added more transparent warnings/error messages about path lengths which exceed the 80-character limit
* Fixed small typo in user guide in installation instructions
* Fixed memory leaks throughout the APBS code
* Fixed NOsh_parseREAD errors for input files with \r line feeds.
* Fixed a variable setting error in the test examples
* Fixed a bug where memory usage is reported incorrectly for large allocations on 64-bit systems
* Added DTRSV to APBS-supplied BLAS to satisfy FEtk SuperLU dependency
* Fixed a small bug in routines.c to print out uncharged molecule id
* Limited calculation of forces when surface maps are read in 

---------------------
APBS 0.5.1 (Jul 2007)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Replaced APOLAR->glen and APOLAR->dime keywords with APOLAR->grid
* Deprecated mergedx. Added mergedx2
    
    * mergedx2 takes the bounding box that a user wishes to calculate a map for, as well as a resolution of the output map. An output map meeting those specifications is calculated and store.
    
* Added pKa tutorial
* Added warning about strange grid settings (MGparm)
* Fixed a bug in vpmg.c that occured when a user supplied a dielectric map with the ionic strength set to zero, causing the map to not be used.
* Removed deprecated (as of 0.5.0) tools/manip/acc in lieu of new APOLAR APBS features
* Added enumerations for return codes, new PBE solver (SMPBE) and linear/ nonlinear types
* Added in code for Size-Modified PBE (SMPBE)


^^^^^^^^^^^^^^^^^^^^^^^^^
Bug fixes and API changes
^^^^^^^^^^^^^^^^^^^^^^^^^

* Fixed buffer over-run problem
* Fixed case inconsistency with inputgen.py and psize.py scripts which caused problems with some versions of Python
* Fixed bug wherein 'bcfl sdh' behaved essentially like 'bcfl zero'.  Now we have the correct behavior:  'bcfl sdh' behaves like 'bcfl mdh' thanks to the multipole code added by Mike Schnieders.  Interestingly, this bug didn't have a major on the large-molecule test cases/examples provided by APBS but did affect the small molecule solvation energies.  Thanks to Bradley Scott Perrin for reporting this bug.
* Added support for chain IDs in noinput.py
* Fixed bug in noinput.py where REMARK lines would cause the script to fail.

---------------------
APBS 0.5.0 (Jan 2007)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Significantly streamlined the configure/build/install procedure:
    
    * Most common compiler/library options now detected by default
    * MALOC is now included as a "plugin" to simplify installation and compatibility issue
    
* Added new APOLAR section to input file and updated documentation -- this function renders tools/manip/acc obsolete.
* Added support for standard one-character chain IDs in PQR files. 
* Added a new "spl4" charge method (chgm) option to support a quintic B-spline discretization (thanks to Michael Schnieders).
* Updated psize.py
* Added a new "spl4" ion-accessibility coefficient model (srfm) option that uses a 7th order polynomial. This option provides the higher order continuity necessary for stable force calculations with atomic multipole force fields (thanks to Michael Schnieders).
* Modified the "sdh" boundary condition (bcfl) option to include dipoles and quadrupoles.  Well-converged APBS calculations won't change with the dipole and quadrupole molecular moments included in the boundary potential estimate, but calculations run with the boundary close to the solute should give an improved result (thanks to Michael Schnieders). 
* Updated documentation to reflect new iAPBS features (NAMD support)
* Added Gemstone example to the tutorial
* New example demonstrating salt dependence of protein-RNA interactions.
* Added code to allow for an interface with TINKER (thanks to Michael Schnieders).
* The Python wrappers are now disabled by default.  They can be compiled by passing the --enable-python flag to the configure script.  This will allow users to attempt to compile the wrappers on various systems as desired.
* Added XML support for reading in parameter files when using PDB files as input.  New XML files can be found in tools/conversion/param/vparam.
* Added XML support for reading "PQR" files in XML format.
* Added support for command line --version and --help flags. 
* Added support for XML output options via the --output-file and  --output-format flags.
* Updated runme script in ion-pmf example to use environmental variable for APBS path
* Modified the license to allow exceptions for packaging APBS binaries with several visualization programs.  PMG license modifed as well.
* Added a DONEUMANN macro to vfetk.c to test FEM problems with all Neumann boundary conditions (e.g., membranes).
* Added Vpmg_splineSelect to select the correct Normalization method with either cubic or quintic (7th order polynomial) spline methods.
* Modified the selection criteria in Vpmg_qfForce, Vpmg_ibForce and Vpmg_dbnpForce for use with the new spline based (spl4) method. 
* Added ion-pmf to the make test suite.
* Updated splash screen to include new PMG acknowledgment
* Added runGB.py and readGB.py to the Python utilities, which calculate solvation energy based on APBS parameterized Generalized Born model.
* Updated authorship and tool documentation
* Deprecated ELEC->gamma keyword in lieu of APOLAR->gamma

^^^^^^^^^^^^^^^^^^^^^^^^^
Bug fixes and API changes
^^^^^^^^^^^^^^^^^^^^^^^^^

* Cleanup of documentation, new Gemstone example
* Clarified usage of dime in mg-para ELEC statements
* Massive cleanup of NOsh, standardizing molecule and calculation IDs and making the serial focusing procedure more robust
* Removed MGparm partOlap* data members; the parallel focusing centering is now done entirely within NOsh
* Updated the user manual and tutorial
* Updated psize.py to use centers and radii when calculating grid sizes (thanks to John Mongan)
* Fixed problems with FEM-based NPBE, LPBE, and LRPBE calculations
* Fixed a minor bug in the configure script that prevented MPI libraries from being found when using certain compilers.
* Updated acinclude.m4, aclocal.m4, config/* for new version (1.9) of automake and compatibility with new MALOC
* Fixed a bug where reading in a file in PDB format had atom IDs starting  at 1 rather than 0, leading to a segmentation fault.
* Fixed a bug in mypde.f where double precision values were initialized with single precision number (causing multiplication errors).
* Fixed a bug in the FEM code. Now calls the npbe solver works properly with FEtk 1.40
* Modified the FEMParm struct to contain a new variable pkey, which is  required for selecting the correct path in AM_Refine

---------------------
APBS 0.4.0 (Dec 2005)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* New version of the 'acc' program available.
* Added additional verbosity to APBS output.
* Added tools/python/vgrid to the autoconf script. The directory compiles with the rest of the Python utilities and is used for manipulating dx files.
* Modified the tools/python/noinput.py example to show the ability to get and print energy and force vectors directly into Python arrays.
* Added dx2uhbd tool to tools/mesh for converting from dx format to UHBD format (Thanks to Robert Konecny)
* Added ability of tools/manip/inputgen.py to split a single mg-para APBS input file into multiple asynchronous input files.
* Modified inputgen.py to be more flexible for developers wishing to directly interface with APBS.
* Added Vclist cell list class to replace internal hash table in Vacc
* Modified Vacc class to use Vclist, including changes to the Vacc interface (and required changes throughout the code)
* Consolidated Vpmg_ctor and Vpmg_ctorFocus into Vpmg_ctor
* Consolidated vpmg.c, vpmg-force.c, vpmg-energy.c, vpmg-setup.c
* Added autoconf support for compilation on the MinGW32 Windows Environment
* Added autoconf support (with Python) for Mac OS 10.4 (Tiger)
* Added the function Vpmg_solveLaplace to solve homogeneous versions of Poisson's equation using Laplacian eigenfunctions.
* Modified the dielectric smoothing algorithm (srfm smol) to a 9 point method based on Bruccoleri, et al.  J Comput Chem 18 268-276 (1997).  NOTE:  This is a faster and more flexible smoothing method.  However, when combined with the the molecular surface bugfixes listed below, this change has the potential to make the srfm smol method give very different results from what was calculated in APBS 0.3.2.  Users who need backwards compatibility are encouraged to use the spline based smoothing method (srfm spl2) or the molecular surface without smoothing (srfm mol).
* Added new 'sdens' input keyword to allow user to control the sphere density used in Vacc.  This became necessary due to the Vacc_molAcc bug fix listed below.  Only applies to srfm mol and srfm smol.
* Made the examples directory documentation much more streamlined.
* Added tests for examples directory.  Users can now issue a "make test" in the desired directory to compare local results with expected results. Also includes timing results for tests for comparison between installations.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Fixed a bug in Vpmg_qmEnergy to remove a spurious coefficient of z_i^2 from the energy calculation.  This generated incorrect results for multivalent ions (but then again, the validity of the NPBE is questionable for multivalents...)  (Big thanks to Vincent Chu)
* Fixed a bug in vacc.c where atoms with radii less than 1A were not considered instead of atoms with no radii.
* Fixed error in tools/mesh/dx2mol.c (Thanks to Fred Damberger)
* Fixed floating point error which resulted in improper grid spacings for some cases.
* Fixed a bug in Vacc_molAcc which generates spurious regions of high internal dielectric for molecular surface-based dielectric definitions.  These regions were very small and apparently affected energies by 1-2% (when used with the 'srfm mol'; the 'srfm smol' can potentially give larger deviations).  The new version of the molecular surface is actually faster (requires 50-70% of the time for most cases) but we should all be using the spline surface anyway -- right? (Thanks to John Mongan and Jessica Swanson for finding this bug).
* Fixed a bug in vpmg.c that caused an assertion error when writing out laplacian maps (Thanks to Vincent Chu).
* Ensured Vpmg::ccf was always re-initialized (in the case where the Vpmg object is being re-used).
* Removed a spurious error estimation in finite element calculations.
* Clarified the role of ccf and other variables in mypde.f and vpmg.c by expanding/revising the inline comments.

---------------------
APBS 0.3.2 (Nov 2004)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* Updated tutorial with more mg-auto examples
* Updated apbs.spec file for generating RPMs on more platforms.
* Added new Python wrapper to tools/python directory showing how to run APBS without PQR and .in inputs.
* Python wrappers are now configured to compile on more architectures/ from more compilers.
* Updated tools/conversion/pdb2pqr to a new version (0.1.0) of PDB2PQR, which now can handle nucleic acids, rebuild missing heavy atoms, add hydrogens, and perform some optimization.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* The dimensions of the fine grids in the pka-lig example calculations were increased to give more reliable results (albeit ones which don't agree with the reported UHBD values as well).
* hz in mgparse.c causes name clash with AIX environmental variable; fixed.
* Fixed documentation to state that using a kappa map does not ignore ELEC ION statements.
* Added a stability fix for printing charge densities for LPBE-type calculations.
* Fixed a bug in NPBE calculations which led to incorrect charge densities and slightly modified total energies.
* Modified the origin when creating UHBD grids to match standard UHBD format.
* Fixed VASSERT error caused by rounding error when reading in dx grid files.

---------------------
APBS 0.3.1 (Apr 2004)
---------------------

^^^^^^^^^^^^
New features
^^^^^^^^^^^^

* New APBS tutorial
* New :file:`tools/python/vgrid/mergedx.py` script to merge dx files generated from parallel APBS runs back into a single dx file.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Fixed bug in parallel calculations where atoms or points on a border between two processors were not included.  Modified setup algorithm for parallel calculations to allow partitions in order to obtain grid points and spacing from the global grid information.
* Modified extEnergy function to work with parallel calculations, giving better accuracy.

---------------------
APBS 0.3.0 (Feb 2004)
---------------------

^^^^
News
^^^^

APBS is now supported by the NIH via NIGMS grant GM69702-01.

^^^^^^^^^^^^^^^^^^^^^^^^^
Changes that affect users
^^^^^^^^^^^^^^^^^^^^^^^^^

* New version of the documentation
* New directory structure in tools/
* Finished fe-manual mode for ELEC calculations -- this is the adaptive finite element solver
* Added documetnation for fe-manual
* New apbs/tools/manip/inputgen.py script to automatically generate input APBS files from PQR data
* Added new asynchronous mode in mg-para parallel calculations to enable running on-demand and/or limited resources
* Added new script (tools/manip/async.sh) to convert mg-para calculations in mg-async calculations
* Added following aliases for some of the more obscure parameters in the input files:

  * chgm 0 ==> chgm spl0
  * chgm 1 ==> chgm spl2
  * srfm 0 ==> srfm mol
  * srfm 1 ==> srfm smol
  * srfm 2 ==> srfm spl2
  * bcfl 0 ==> bcfl zero
  * bcfl 1 ==> bcfl sdh
  * bcfl 2 ==> bcfl mdh
  * bcfl 4 ==> bcfl focus
  * calcenergy 0 ==> calcenergy no
  * calcenergy 1 ==> calcenergy total
  * calcenergy 2 ==> calcenergy comps
  * calcforce 0 ==> calcforce no
  * calcforce 1 ==> calcforce total
  * calcforce 2 ==> calcforce comps

* Example input files have been updated to reflect this change. NOTE: the code is backward-compliant; i.e., old input files WILL still work.
* Added new READ options "PARM" and "MOL PDB", see documentation for more information. These options allow users to use unparameterized PDB files together with a parameter database.
* Updated the documentation
* Now include support for chain IDs and other optional fields in PQR/PDB files
* Added support for parsing PDB files
* Renamed:

* amber2charmm -> amber2charmm.sh
* pdb2pqr -> pdb2pqr.awk
* qcd2pqr -> qcd2pqr.awk

* Added a new Python-based pdb2pqr (tools/conversion/pdb2pqr) script that allows users to choose parameters from different forcefields.
* Updated Python wrappers (tools/python) and added the python directory to autoconf/automake.
* Reformatted examples/README.html for readability.

^^^^^^^^^
Bug fixes
^^^^^^^^^

* Fixed bug in PQR parsing that can cause PDB/PQR files to be mis-read when they contain residues with numbers in their names (Thanks to Robert Konecny and Joanna Trylska)
* Fixed bug when writing out number/charge density: unrealistic densities reported inside iVdW surface.
* Fixed bug in VMD read_dx utility
* Invalid map IDs now result in an error message instead of a core dump (thanks to Marco Berrera)
* Modified mechanism for cubic-spline output, fixing a bug associated with zero-radius atoms
* Fixed omission of srfm in sections of documentation (thanks to Sameer Varma)
* Made autoconf/automake configure setup more robust on Solaris 8 platforms (thanks to Ben Carrington)
   
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changes that affect developers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* New docuemtnation setup
* New tools/ directory structure
* Changed Vgreen interface and improved efficiency
* Changed Vopot interface to support multiple grids
* Added several norm and seminorm functions to Vgrid class
* Altered --with-blas syntax in configure scripts and removed --disable-blas
* Documented high-level frontend routines
* Cool new class and header-file dependency graphs courtesy of Doxygen and Graphviz
* Added substantial mypde.c-based functionality to Vfetk
* Moved chgm from PBEparm to MGparm
* Minor changes to Vfetk: removed genIcos and added genCube
* FEM solution of RPBE working again (see test/reg-fem) and is probably more up-to-date than test/fem
* Updated API documentation
* Changed many NOsh, FEMparm, MGparm variables to enums
* Changes to Valist and Vatom classes
* Fixed minor bugs in documentation formatting
* Made Vopot more robust
* Created Vparam class for parameter file parsing
* Added vparam* parameter database flat files to tools/conversion/param

---------------------
APBS 0.2.6 (Jan 2003)
---------------------

* Changed license to GPL
* Made a few routines compliant with ANSI X3.159-1989 by removing snprintf (compliant with ISO/IEC 9899:1999).  This is basically for the sake of OSF support.

---------------------
APBS 0.2.5 (Nov 2002)
---------------------

* Improved consistency between energies evaluated with "chgm 0" and "chgm 1"
* Made charge-field energy evaluation consistent for user-supplied charge maps
* Added new psize.py script courtesy of Todd Dolinsky.
* Updated list of APBS-related tools in User Guide.
* Added RPM capabilities courtesy of Steve Bond.
* Removed annoying excess verbosity from Vgrid.
* Updated Blue Horizon compilation instructions (thanks to Robert Konecny and Giri Chukkapalli)
* Updated autoconf/automake/libtool setup and added --disable-tools option

---------------------
APBS 0.2.4 (Oct 2002)
---------------------

* Fixed bug which set one of the  z-boundaries to zero for "bcfl 1".  This can perturb results for systems where the grid boundaries are particularly close to the biomolecule.  While this is an embarassing bug, most systems using settings suggested by the psize script appear largely unaffected (see examples/README.html).  Thanks to Michael Grabe for finding this bug (Michael, you can stop finding bugs now...)
* Updated VMD scripts to agree with the current OpenDX output format
* A COMMENT:  As far as I can tell, the current version of OpenDX-formatted output (same as version 0.2.3) is fully compliant with the OpenDX standards (see, for example,  http://opendx.npaci.edu/docs/html/pages/usrgu065.htm#HDRXMPLES).   However, I realize this is different than the format for previous versions and would encourage all users to update their APBS-based applications to accomodate these changes.  The best solution would be for all downstream applications to use the APBS Vgrid class (see http://agave.wustl.edu/apbs/doc/api/html/group__Vgrid.html) to manipulate the data since this class should remain internally consistent between releases.  Finally, I would love to have some OpenDX guru who uses APBS to contact me so I can solidfy the data ouput format of APBS.  I'm about ready to permanently switch to another format if I can't reach a consensus with OpenDX...

---------------------
APBS 0.2.3 (Oct 2002)
---------------------

* Fixed bugs in salt-dependent Helmholtz/nonlinear term of PBE affecting both LPBE and NPBE calculations.  While this bug fix only changes most energies by < 2 kJ/mol, it is recommended that all users upgrade.  Many thanks to Michael Grabe for finding and carefully documenting this bug!
* A parameter (chgm) has been added which controls the charge discretization method used.  Therefore, this version contains substantial changes in both the API and input file syntax.  Specifically:
    
    * PBEparm has two new members (chgm, setchgm)
    * Vpmg_fillco requires another argument
    * Vpmg\_*Force functions require additional arguments
    * Input files must now contain the keyword "chgm #" where # is an integer
    * Please see the documentation for more information.
    
* Fixed problems with "slicing" off chunks of the mesh during I/O of focused calculations
* Updated authors list
* New CHARMM parameters -- Robert Konecny
* Created enumerations for common surface and charge discretization methods
* Added Vmgrid class to support easy manipulation of nested grid data
* Added more verbosity to error with NPBE forces
* Added working Python wrappers -- Todd Dolinksy
* Modified VMD scripts read_dx and loadstuff.vmd

---------------------
APBS 0.2.2 (Aug 2002)
---------------------

* There were several other changes along the way... I lost track.
* Changed coordinate indexing in some energy calculations
* Updated documentation to reflect recent changes on Blue Horizon
* Improved speed of problem setup BUT NOW RESTRICT use of input coefficient maps (see documentation)
* Updated documentation, placing particular emphasis on use of Intel compilers and vendor BLAS on Intel Linux systems
* Fixed bug for nonpolar force evaluation in Vpmg_dbnpForce
* Removed MG test scripts; use :file:`bin/*.c` for templates/testing
* Made main driver code completely memory-leak free (i.e., if you wanted to wrap it and call it repeatedly -- Thanks to Robert Konecny)
* Fixed main driver code for compatibility with SGI compilers (Thanks to Fabrice Leclerc)
* Made focused evaluation of forces more sensible.
* Added 'print force' statement
* Fixed bug in OpenDX input/output (OpenDX documentation is lousy!)

---------------------
APBS 0.2.1 (Apr 2002)
---------------------

This version requires the latest version of MALOC to work properly!

* Syntax changes
    
    * The writepot and writeacc keywords have been generalized and new I/O features have been added.  The syntax is now:
        
        * write pot dx potential
        * write smol dx surface
        * etc.  Please see the User's Manual for more information
        
    * The read keywords has been generalized and new I/O features have been added which support the use of pre-calculated coefficient grids, etc.  The correct syntax for reading in a molecule is now "read mol pqr mol.pqr end"; please see the User's Manual for more information.
    * The "mg" keyword is no longer supported; all input files should use "mg-manual" or one of the other alternatives instead.
    
* A change in the behavior of the "calcenergy" keyword; passing an argument of 2 to this keyword now prints out per-atom energies in addition to the energy component information
* A new option has been added to tools/manip/acc to give per-atom solvent-accessible surface area contributions
* A new option has been added to tools/manip/coulomb to give per-atom electrostatic energies
* Added tools/mesh/dxmath for performing arithmetic on grid-based data (i.e., adding potential results from two calculations, etc.)
* Added tools/mesh/uhbd_asc2bin for converting UHBD-format grid files from ASCII to binary (contributed by Dave Sept)
* Improvement of VMD visualization scripts (contributed by Dave Sept)
* The API has changed significantly; please see the Programmer's Manual.
* Working (but still experimental) Python wrappers for major APBS functions.
* More flexible installation capabilities (pointed out by Steve Bond)
* Added ability to use vendor-supplied BLAS
* Brought up-to-date with new MALOC

---------------------
APBS 0.2.0 (Mar 2002)
---------------------

This version is a public (beta) release candidate and includes:

* Slight modification of the user and programmer's guides
* Scripts for visualization of potential results in VMD (Contributed by Dave Sept)
* Corrections to some of the example input files
* A few additional API features

This release requires a new version of MALOC. 

---------------------
APBS 0.1.8 (Jan 2002)
---------------------

This version is a public (beta) release candidate and includes the following bug-fixes:

* Added warning to parallel focusing 
* Added several test cases and validated the current version of the code for all but one (see examples/README.html)
* Fixed atom partitioning bug and external energy evaluation during focusing
* Added new program for converting OpenDX format files to MOLMOL (by Jung-Hsin Lin)

You should definitely upgrade, the previous versions may produce unreliable results.

---------------------
APBS 0.1.7 (Dec 2001)
---------------------

This version is a public (beta) release candidate and includes the following bug-fixes:

* Fixed I/O for potential in UHBD format (thanks, Richard!)
* Re-arranged garbage collection routines in driver code
* Improved FORTRAN/C interfaces
* Re-configured autoconf/libtool setup for more accurate library version number reporting

---------------------
APBS 0.1.6 (Nov 2001)
---------------------

This version is a public (beta) release candidate and includes the following bug-fixes and features:

* Fixed printf formatting in UHBD potential output
* Added input file support for parallel focusing
* Fixed small bug in parsing writeacc syntax (thanks, Dave)
* Added output file support for parallel focusing
* Changed some documentation

You need to download a new version of MALOC for this release.   

---------------------
APBS 0.1.5 (Oct 2001)
---------------------

This version features minor bug fixes and several new features:

* Fixed shift in center of geometry for OpenDX I/O
* Made energy evaluation more robust when using NPBE
* Rearrangments of files and modified compilation behavior
* Input file support for ion species of varying valency and concentration
* Input file support incorrect nlev/dime combinations; APBS now finds acceptable settings near to the user's requested values
* "Automatic focusing".  Users now simply specify the physical parameters (temperature, dielectric, etc.), the coarse and fine grid lengths and centers, and APBS calculates the rest

---------------------
APBS 0.1.4 (Sep 2001)
---------------------

This version features major bug fixes introduced in the 0.1.3 release:

* Chain ID support has been **removed** from the PDB/PQR parser (if anyone has a nice, flexible PDB parser they'd like to contribute to the code, I'd appreciate it)
* Configure script has been made compatible with OSF
* Bug fix in disabling FEtk-specific header files

---------------------
APBS 0.1.3 (Sep 2001)
---------------------

This version features a few improvements in scripts, PDB parsing flexibility, and portability, including:

* Dave Sept upgraded the psize and shift scripts to allow more flexibility in PDB formats.
* Chain ID support has been added to the PDB/PQR parser
* Removed -g from compiler flags during linking of C and FORTAN under OSF (thanks to Dagmar Floeck and Julie Mitchell for help debugging this problem)

---------------------
APBS 0.1.2 (Sep 2001)
---------------------

This version is mainly designed to increase portability by switching to libtool for library creation and linking.
Of course, it also contains a few bug fixes.
Highlights include:

* Changes to the User Manual
* Addition of a Programmer's Manual
* Various FEtk-related things (no particular impact to the user)
* Improvements to the test systems
* Change in the format for printing energies
* Change in directory structure
* Fixed centering bug in main driver (only impacted I/o)
* Fixed error message bug in VPMG class
* Fixed grid length bug (popped up during sanity checks) in VPMG class
* Switched to libtool for linking
* Note that Compaq Tru64 Alpha users may still experience problems while compiling due to some strangess with linking C and FORTRAN objects.

---------------------
APBS 0.1.1 (Aug 2001)
---------------------

I am slightly less pleased to announce the first bug-fix for APBS, version 0.1.1.
This fixes compilation problems that popped up for several folks,
including:

* Syntax errors with non-GNU compilers
* Errors in the installation instructions
* Installation of binary in machine-specific directory

---------------------
APBS 0.1.0 (Aug 2001)
---------------------

I am pleased to announce the availability of a pre-beta version of the Adaptive Poisson-Boltzmann Solver (APBS) code to selected research groups.
APBS is new software designed to solve the Poisson-Boltzmann equation for very large biomolecular systems.
For more information, please visit the APBS web site at http://mccammon.ucsd.edu/apbs.

This release is designed to allow interested users to get familiar with the code. 
It is not currently fully functional; it only provides for the sequential multigrid (Cartesian mesh) solution of the linearized and nonlinear Poisson-Boltzmann equation.
User-friendly parallel support will be incorporated into the next release.
Other limitations that may impact its immediate usefulness are:

* No finite element support.  This is awaiting the public release of the Holst group's FEtk library.
* Somewhat inefficient coefficient evaluation (i.e., problem setup).  This should be fixed in the next release or two.

Rather than serving as a production code, this release represents a request for help in breaking the software and finding its deficiencies
before a public beta.

If you are interested in testing this early release, please go to http://wasabi.ucsd.edu/~nbaker/apbs/download/.
Since this is not a public release of APBS, you will need to enter the user-name "apbs-beta" and the password "q94p$fa!" for access to this site.
Once there, please follow the instructions to download and install APBS.

If you are not interested in trying out this early release, but would like to stay informed about subsequent versions of APBS, please consider subscribing to the APBS announcements mailing list by sending the message "subscribe apbs-announce" to majordomo@mccammon.ucsd.edu.

Thank you for your time and interest in the APBS software.

