APBS Release Procedure
-----------------------
 1. Change Version Number
	 - [ ] Edit [CMakeLists.txt]([https://github.com/Electrostatics/apbs/blob/master/apbs/CMakeLists.txt)
		Increment the value for the APBS_VERSION variable:
	     set(APBS_VERSION "M.m.u")
	     Where:
		 - M is the Major version - increment if there are breaking changes or dropping support for previous features
		 - m is the Minor version - increment for new features added
		 - u is the Micro version - increment for adding small changes like new tests or fixing small bugs

 2. Update the ChangeLog
	 - [ ] Edit [doc/ChangeLog]([https://github.com/Electrostatics/apbs/blob/master/apbs/doc/ChangeLog.md)
	   - Document major/minor changes for this release
   
  3. Update License info
	   - [ ] Update license dates and information in source files
	   - In apbs/src edit all .c source files and all .h header files, update dates
  
 * Test release
   - Set up separate machines or virtual machines for target deploy platforms:
	 - [ ] Ubuntu-latest 
	 - [ ] MacOSX-latest
	 - [ ] Windows-latest (Windows Subsystem Linux)
     
   - On testing platforms install or verify presence of required tools:
	 - [ ] Essential compile toolchain 
     - [ ] Python 3.6 or newer
     - [ ] git
     - [ ] CMake 3.12 or newer
     - [ ] Doxygen
     - [ ] LaTeX builder like texlive, tetex (usually already available in linux)
     
   - Clone apbs github repository to testing machines:
```bash
            #!/bin/bash
            mkdir ~/git
            cd ~/git
            git clone https://github.com/Electrostatics/apbs.git
            cd apbs
            git submodule init
            git submodule update
```
   - Build testing
     - On each platform test machine us the script below to build APBS
     - Ensure that all tests run without segmentation faults and results are acceptable

```bash
		run ./.build.sh
```
     
 * Upload Binary Packages
   - On the following test platforms:
	 - [ ] Ubuntu-latest 
	 - [ ] MacOSX-latest
	 - [ ] Windows-latest (Windows Subsystem Linux)
     
   - Add entire install structure to archive file
```bash
	     #!/bin/bash
	     VERSION=`cat ~/git/apbs/VERSION | sed -e "s/_/./g"`
	     PLATFORM=`uname -s`
	     ARCHITECTURE=`uname -m`
	     tar -czf APBS-${VERSION}-${PLATFORM}-${ARCHITECTURE}.tgz ~/apbs-install
```
   - Upload the archive to apbs project on https://sourceforge.net/projects/apbs/files/apbs/
     
 * Upload Source Packages
```bash
	     #!/bin/bash
	     VERSION=`cat ~/git/apbs/VERSION | sed -e "s/_/./g"`
	     cd ~/git/apbs/apbs
	     #  Use git to remove all non-versioned files and directories
	     #  NOTE: Use -dfn flags first for a dry run and make sure right files are rm'ed
	     git clean -dfq
	     #  NOTE: Add the whole apbs directory to an arcive
	     tar -czf APBS-${VERSION}-source.tar.gz ~/git/apbs
```
   - Upload the archive to apbs project on https://sourceforge.net/projects/apbs/files/apbs/
     
 * Upload Package Programmer Guide Package
   - Build documentation
```bash
	     #!/bin/bash
	     VERSION=`cat ~/git/apbs/VERSION | sed -e "s/_/./g"`
	     cd ~/git/apbs/apbs/doc
	     #  Use git to remove all non-versioned files and directories
	     #  NOTE: Use -dfn flags first for a dry run and make sure right files are rm'ed
	     git clean -dfq
	     #  NOTE: Add the whole apbs directory to an arcive
	     tar -czf APBS-${VERSION}-programmer_guide.tgz
```
     
   - Upload the archive to apbs project on https://sourceforge.net/projects/apbs/files/apbs/

 * Update http://www.poissonboltzmann.org/apbs/release-history with new release information.
<!--stackedit_data:
eyJoaXN0b3J5IjpbNTk4MzY2NDQ1XX0=
-->