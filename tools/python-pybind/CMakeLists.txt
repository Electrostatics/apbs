
find_package(pybind11 REQUIRED)
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
