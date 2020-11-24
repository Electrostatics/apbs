################################################################################
# TODO:  Generate license.html
################################################################################

file(READ conf.py.in CONFPYDATA)
message(STATUS "Update RTD conf.py to VERSION ${CMAKE_PROJECT_VERSION}")
STRING(REGEX REPLACE __PROJECT_VERSION__ ${CMAKE_PROJECT_VERSION} CONFPYDATA ${CONFPYDATA})
message(STATUS "Update RTD conf.py to YEAR ${COMPILE_YEAR}")
STRING(REGEX REPLACE __YEAR__ ${COMPILE_YEAR} CONFPYDATA ${CONFPYDATA})
file(WRITE conf.py ${CONFPYDATA})


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
            DESTINATION ${SHARE_INSTALL_PATH}/docs
            PATTERN "Doxyfile" EXCLUDE
            PATTERN "latex" EXCLUDE
			PATTERN "mainpage.h" EXCLUDE
		)

	endif(Doxygen_FOUND)

endif()