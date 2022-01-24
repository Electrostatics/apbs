################################################################################
# TODO:  Generate license.html
################################################################################

################################################################################
# NOTE: Don't do this unless you have LaTex and way too much time to kill
################################################################################
option(BUILD_DOXYGEN "Build/Rebuild documentation using doxygen" OFF)

if(BUILD_DOXYGEN)

	find_package(Doxygen)

	if(Doxygen_FOUND)
		configure_file(
	    	${CMAKE_CURRENT_SOURCE_DIR}/programmer/.config/Doxyfile.in
	    	${CMAKE_CURRENT_BINARY_DIR}/programmer/Doxyfile
	    	ESCAPE_QUOTES
	    	)
	
		configure_file(
	    	${CMAKE_CURRENT_SOURCE_DIR}/programmer/.config/programmer.html.in
	    	${CMAKE_CURRENT_BINARY_DIR}/programmer/programmer.html
	    	)
	
		configure_file(
	    	${CMAKE_CURRENT_SOURCE_DIR}/programmer/.config/mainpage.h.in
	    	${CMAKE_CURRENT_BINARY_DIR}/programmer/mainpage.h
	    	)
	
		add_custom_target(
	    	doc
	    	ALL
	    	#COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/programmer/Doxyfile
	    	COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/programmer/Doxyfile
	    	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/programmer
	    	COMMENT "Generating documentation with Doxygen" VERBATIM
	  	)

		#file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/programmer/latex)
	
		add_custom_target(
	    	doc-pdf
	    	ALL
	    	COMMAND make
	    	COMMAND cp refman.pdf ../programmer.pdf
	    	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/programmer/latex
	    	DEPENDS doc
	    	COMMENT "Generating pdf documentation" VERBATIM
		)

    	install(DIRECTORY ${APBS_BUILD}/docs/programmer
            DESTINATION ${CMAKE_INSTALL_DATADIR}/docs
            PATTERN "Doxyfile" EXCLUDE
            PATTERN "latex" EXCLUDE
			PATTERN "mainpage.h" EXCLUDE
            COMPONENT docs
		)

	endif(Doxygen_FOUND)

endif()

################################################################################
# TODO:  Generate RTD for the APBS src
################################################################################
option(BUILD_BREATHE "Build/Rebuild documentation using Sphinx/Breathe for APBS src" OFF)

# This is part of issue https://github.com/Electrostatics/apbs/issues/41
# Based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

message(STATUS "================================================================")
if(BUILD_BREATHE)
  message(STATUS "BUILD RTD using Sphinx/Breathe")
  find_package(Doxygen REQUIRED)

  include(FindPackageHandleStandardArgs)
  find_program(Sphinx_EXECUTABLE NAMES sphinx-build)

  # Find all the public headers
  file(GLOB_RECURSE APBS_SRC_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/*.h)

  set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
  set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  # Replace variables inside @@ with the current values
  #configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  # Doxygen won't create this for us
  file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

  # Only regenerate Doxygen when the Doxyfile or public headers change
  add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                     DEPENDS ${APBS_SRC_PUBLIC_HEADERS}
                     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                     MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                     COMMENT "Generating docs"
                     VERBATIM)

  # Nice named target so we can run the job easily
  add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

  set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
  set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
  set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

  # Only regenerate Sphinx when:
  # - Doxygen has rerun
  # - Our doc files have been updated
  # - The Sphinx config has been updated
  add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                     COMMAND 
                       ${Sphinx_EXECUTABLE} -b html
                       # - Tell Breathe where to find the Doxygen output
                       -Dbreathe_projects.src=${DOXYGEN_OUTPUT_DIR}/xml
                       ${SPHINX_SOURCE} ${SPHINX_BUILD}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     DEPENDS
                     # Other docs files you want to track should go here (or in some variable)
                       ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
                       ${DOXYGEN_INDEX_FILE}
                     MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                     COMMENT "Generating documentation with Sphinx")

  # Nice named target so we can run the job easily
  add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

  # Add an install target to install the docs
  include(GNUInstallDirs)
  install(DIRECTORY ${SPHINX_BUILD} DESTINATION ${CMAKE_INSTALL_DOCDIR} COMPONENT docs)

endif()
message(STATUS "================================================================")