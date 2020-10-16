
find_package(pybind11)

add_library(pybind11_dep INTERFACE)
if(NOT TARGET pybind11::module)
  ExternalProject_add(pybind11
    PREFIX pybind11
    GIT_REPOSITORY "https://github.com/pybind/pybind11.git"
    GIT_TAG "v2.5.0"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
  target_include_directories(pybind11_dep INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/pybind11/src/pybind11/include)
else()
  target_link_libraries(pybind11_dep INTERFACE pybind11::module)
endif()

target_include_directories(pybind11_dep INTERFACE ${Python_INCLUDE_DIRS})

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
  pybind11_dep
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
