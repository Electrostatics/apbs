
find_package(pybind11)

add_library(pybind11_dep INTERFACE)
if(NOT TARGET pybind11::module)
  target_include_directories(pybind11_dep INTERFACE
    ${PROJECT_SOURCE_DIR}/externals/pybind11/include)
  include(${PROJECT_SOURCE_DIR}/externals/pybind11/tools/FindPythonLibsNew.cmake)
else()
  target_link_libraries(pybind11_dep INTERFACE pybind11::module)
endif()

target_include_directories(pybind11_dep INTERFACE ${Python3_INCLUDE_DIRS} $ENV{Python3_INCLUDE_DIRS})
# target_link_libraries(pybind11_dep INTERFACE ${Python3_LIBRARIES})

add_library(apbs_pybind
  MODULE
  module.cpp
  bind_nosh.cpp
  bind_vatom.cpp
  bind_valist.cpp
  )

message(STATUS "Site pkgs ${PROJECT_BINARY_DIR}/${Python3_SITELIB}")
set_target_properties(apbs_pybind
  PROPERTIES
  PREFIX "${PYTHON_MODULE_PREFIX}"
  SUFFIX "${PYTHON_MODULE_EXTENSION}"
  OUTPUT_NAME "apbs_pybind"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

if(APPLE)
  set_target_properties(apbs_pybind
    PROPERTIES
    COMPILE_FLAGS "-undefined dynamic_lookup"
    )
endif()


message(STATUS "LIBS ${APBS_LIBS}")
message(STATUS "INTERNAL_LIBS ${APBS_INTERNAL_LIBS}")
target_link_libraries(
  apbs_pybind
  PRIVATE
  pybind11_dep
  ${APBS_LIBS}
  ${APBS_INTERNAL_LIBS}
  )

add_custom_command(
  TARGET apbs_pybind
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:apbs_pybind>
    ${PROJECT_SOURCE_DIR}/apbs/bindings/$<TARGET_FILE_NAME:apbs_pybind>
  )

# install(TARGETS apbs_pybind DESTINATION lib)
# if(PYTHON_SITE_PACKAGES_DIR)
  # install(TARGETS apbs_pybind DESTINATION ${Python3_SITELIB})
# else()
  # message(STATUS "Python site-packages dir not specified.")
  # message(STATUS "apbs_pybind module will likely not be on your \
    # PYTHONPATH unless you add ${CMAKE_INSTALL_PREFIX}/lib manually.")
# endif()
