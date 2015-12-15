FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SWIG_ADD_MODULE(apbslib python apbslib.i)

# Hack to get the correct shared library extensions.  Why does Python suck so hard wrt to this?
if(APPLE)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

# Another hack to get SWIG to use the the correct library extension!
SET_TARGET_PROPERTIES(${SWIG_MODULE_apbslib_REAL_NAME} PROPERTIES SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})

SWIG_LINK_LIBRARIES(apbslib ${APBS_LIBS} ${APBS_INTERNAL_LIBS} ${PYTHON_LIBRARIES})

add_custom_command(TARGET _apbslib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
        copy ${CMAKE_CURRENT_BINARY_DIR}/apbslib.py ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
    COMMAND ${CMAKE_COMMAND} -E
        copy ${LIBRARY_OUTPUT_PATH}/_apbslib${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_SOURCE_DIR}/../pdb2pqr/pdb2pka
)
