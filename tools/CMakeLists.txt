set(EXECUTABLE_OUTPUT_PATH ${TOOLS_PATH}/bin)

add_subdirectory(mesh)
add_subdirectory(manip)

if(ENABLE_PYTHON)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  include(FindAPBSPythonLibraries)
  include(APBSWrapPythonLibraries)

  # Temporarily remove swig bindings for appveyor build as of commit
  # be2fe43. This should be uncommented before deployment.
  # add_subdirectory(python)
  add_subdirectory(python_pybind)

  apbs_wrap_python_libs(
    TARGETS
    # _apbslib
    apbs_pybind
    )
endif(ENABLE_PYTHON)

