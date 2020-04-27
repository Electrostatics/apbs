if (${CMAKE_VERSION} VERSION_GREATER "3.13")
	CMAKE_POLICY(SET CMP0078 OLD)
	CMAKE_POLICY(SET CMP0086 OLD)
endif()

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})


find_package(Python3 COMPONENTS Interpreter Development)
if(${Python3_FOUND})
    #message(STATUS "******* Python3 FOUND")
endif()

INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
message(STATUS "***** Python include path is: ${Python3_INCLUDE_DIRS}")
message(STATUS "***** Python library path is: ${Python3_LIBRARIES}")
message(STATUS "***** Python library dir  is: ${Python3_LIBRARY_DIRS}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SWIG_ADD_LIBRARY(apbslib
		#TYPE USE_BUILD_SHARED_LIBS
		TYPE SHARED
		LANGUAGE python
		SOURCES apbslib.i)

# Hack to get the correct shared library extensions.  Why does Python suck so hard wrt to this?
if(APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

# Another hack to get SWIG to use the the correct library extension!
#SET_TARGET_PROPERTIES(${SWIG_MODULE_apbslib_REAL_NAME} PROPERTIES SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})

SWIG_LINK_LIBRARIES(apbslib ${APBS_LIBS} ${APBS_INTERNAL_LIBS} ${Python3_LIBRARIES})

if(WIN32)
	add_custom_command(TARGET _apbslib POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
			copy ${CMAKE_CURRENT_BINARY_DIR}/apbslib.py ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
		COMMAND ${CMAKE_COMMAND} -E
			copy ${LIBRARY_OUTPUT_PATH}/$<CONFIG>/_apbslib.pyd ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
		)
else()
	add_custom_command(TARGET _apbslib POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
			copy ${CMAKE_CURRENT_BINARY_DIR}/apbslib.py ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
		COMMAND ${CMAKE_COMMAND} -E
			copy ${LIBRARY_OUTPUT_PATH}/_apbslib${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
	)
endif()
