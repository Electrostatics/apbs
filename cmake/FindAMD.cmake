################################################################################
# [FETK header]
# Adapted from eigen/cmake/FindUMFPACK.cmake
# https://gitlab.com/libeigen/eigen/-/blob/master/cmake/FindUMFPACK.cmake
# Accessed on June 18, 2021
################################################################################

if (AMD_INCLUDES AND AMD_LIBRARIES)
  set(AMD_FIND_QUIETLY TRUE)
endif ()

find_path(AMD_INCLUDES
  NAMES
  amd.h
  PATHS
  $ENV{AMDDIR}
  ${INCLUDE_INSTALL_DIR}
  PATH_SUFFIXES
  suitesparse
  ufsparse
)

find_library(AMD_LIBRARIES 
  NAMES amd libamd 
  PATHS $ENV{AMDDIR} ${LIB_INSTALL_DIR}
)

if(AMD_LIBRARIES AND NOT AMD_LIBDIR)
    get_filename_component(AMD_LIBDIR ${AMD_LIBRARIES} PATH)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AMD DEFAULT_MSG
                                  AMD_INCLUDES AMD_LIBRARIES AMD_LIBDIR)

mark_as_advanced(AMD_INCLUDES AMD_LIBRARIES AMD_LIBDIR)
