set(EXECUTABLE_OUTPUT_PATH ${TOOLS_PATH}/bin)

add_subdirectory(mesh)
add_subdirectory(manip)

if(ENABLE_PYTHON)
    add_subdirectory(python)
endif(ENABLE_PYTHON)
