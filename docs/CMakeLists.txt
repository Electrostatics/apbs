################################################################################
# TODO:  Generate license.html
################################################################################

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
endif(Doxygen_FOUND)
