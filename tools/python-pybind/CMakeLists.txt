
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG QUIET)
if(NOT TARGET pybind11::module)
  if(NOT EXISTS ${CMAKE_SOURCE_DIR}/externals/pybind11/CMakeLists.txt)
    message(FATAL_ERROR "pybind11 was not found and its submodule is unavailable")
  endif()
  add_subdirectory(
    ${CMAKE_SOURCE_DIR}/externals/pybind11
    ${CMAKE_BINARY_DIR}/externals/pybind11
    EXCLUDE_FROM_ALL
    )
endif()

add_library(apbs_pybind
  MODULE
  module.cpp
  bind_nosh.cpp
  bind_vatom.cpp
  bind_valist.cpp
  )

set_target_properties(apbs_pybind
  PROPERTIES
  PREFIX "${PYTHON_MODULE_PREFIX}"
  SUFFIX "${PYTHON_MODULE_EXTENSION}"
  OUTPUT_NAME "apbs"
  )

message(STATUS "LIBS ${APBS_LIBS}")
message(STATUS "INTERNAL_LIBS ${APBS_INTERNAL_LIBS}")
target_link_libraries(
  apbs_pybind
  PRIVATE
  pybind11::module
  ${APBS_LIBS}
  ${APBS_INTERNAL_LIBS}
  )

install(TARGETS apbs_pybind LIBRARY DESTINATION lib COMPONENT python)
