APBS Release Procedure
-----------------------
 1. Change Version Number
	 - [ ] Edit [VERSION]([https://github.com/Electrostatics/apbs/blob/main/apbs/VERSION)
		Increment the value after the comment block, which is in the form
	     M_m_u
	     Where:
		 - M is the Major version - increment if there are breaking changes or dropping support for previous features
		 - m is the Minor version - increment for new features added
		 - u is the Micro version - increment for adding small changes like new tests or fixing small bugs

 2. Update the Releases document
	 - [ ] Edit [docs/releases.rst]([https://github.com/Electrostatics/apbs/blob/main/apbs/docs/releases.rst)
	   - Document major/minor changes for this release
   
 3. Update License info
	   - [ ] Update license dates and information in source files
	   - In apbs/src edit all .c source files and all .h header files, update dates
  
 4. Create a Pull Request (PR)
     - [ ] Create a new [Pull Request](https://github.com/Electrostatics/apbs/pulls)
		 - Base branch should be `release`
		 - Source branch should be `main`
		 - Briefly describe the changes included

 5. Check tests
     - Go to the [Actions](https://github.com/Electrostatics/apbs/actions) tab in GitHub
     - Tests are performed for three target platforms:
       - Ubuntu
	   - MacOSX
	   - Windows
     - [ ] Ensure that the builds and associated tests were successful
	 - [ ] Ensure that the use tests were successful
	 - [ ] Ensure that the build artifacts were uploaded to the action

 6. Merge the PR
	 - [ ] Ensure that the [Release](https://github.com/Electrostatics/apbs/releases) is correctly created
	 - [ ] Ensure that the builds and associated tests were successful
	 - [ ] Ensure that the use tests were successful
	 - [ ] Ensure that the build artifacts were uploaded to the Release

 7. Update http://www.poissonboltzmann.org/apbs/release-history with new release information.

 8. Pat yourself on the back for a job well done!
