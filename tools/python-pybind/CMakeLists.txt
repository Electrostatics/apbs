
ExternalProject_add(pybind11
  PREFIX pybind11
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/externals/pybind11"
  CMAKE_CACHE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/pybind11
  )

# find_package(pybind11 REQUIRED)
add_library(pybind11 INTERFACE)
target_include_directories(pybind11 INTERFACE ${PROJECT_BINARY_DIR}/pybind11}

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
  OUTPUT_NAME "apbs_pybind"
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

install(TARGETS apbs_pybind LIBRARY DESTINATION lib)
if(PYTHON_SITE_PACKAGES_DIR)
  install(TARGETS apbs_pybind LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES_DIR})
else()
  message(STATUS "Python site-packages dir not specified.")
  message(STATUS "apbs_pybind module will likely not be on your \
    PYTHONPATH unless you add ${CMAKE_INSTALL_PREFIX}/lib manually.")
endif()
