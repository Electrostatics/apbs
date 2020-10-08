
find_package(pybind11)

add_library(pybind11_dep INTERFACE)
if(NOT TARGET pybind11::module)
  target_include_directories(pybind11_dep INTERFACE
    ${PROJECT_SOURCE_DIR}/externals/pybind11/include)
  include(${PROJECT_SOURCE_DIR}/externals/pybind11/tools/FindPythonLibsNew.cmake)
else()
  target_link_libraries(pybind11_dep INTERFACE pybind11::module)
endif()

target_include_directories(pybind11_dep INTERFACE ${Python3_INCLUDE_DIRS})
if(WIN32 OR WIN64)
  target_link_libraries(pybind11_dep INTERFACE ${Python3_LIBRARIES})
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

add_custom_command(
  TARGET apbs_pybind
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  $<TARGET_FILE:apbs_pybind>
  ${PROJECT_SOURCE_DIR}/apbs/bindings/$<TARGET_FILE_NAME:apbs_pybind>
  )
