
# Applies options to each library for importing into python
macro(apbs_wrap_python_libs)
  cmake_parse_arguments(APBS_WRAP_PYTHON_LIBS
    ""
    ""
    "TARGETS"
    ${ARGN}
    )

  foreach(target ${APBS_WRAP_PYTHON_LIBS_TARGETS})
    message(STATUS "Configuring options for APBS python library: ${target}")

    # Allow undefined symbols on OSX since we don't want to link against
    # libpython. See this thread for reference:
    # https://sourceforge.net/p/swig/mailman/swig-user/thread/CAEppYpHSOM_fUyxDL8-MCETn-i8F94JjUdDc3%3D%3D2GejvqwHY8A%40mail.gmail.com/#msg36158066
    #
    # And near the bottom on this thread:
    # https://pybind11.readthedocs.io/en/stable/compiling.html
    if(APPLE)
      set_target_properties(${target}
        PROPERTIES
        PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
        COMPILE_FLAGS "-undefined dynamic_lookup"
        LINK_FLAGS "-undefined dynamic_lookup"
        )
    elseif(WIN32 OR WIN64)
      target_link_libraries(${target} PRIVATE ${Python3_LIBRARIES})
    endif()

    set_target_properties(${target}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      )
  endforeach()
endmacro(apbs_wrap_python_libs)
