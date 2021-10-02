################################################################################
# [FETK header]
# From eigen/cmake/FindUMFPACK.cmake
# https://gitlab.com/libeigen/eigen/-/blob/master/cmake/FindUMFPACK.cmake
# Accessed on June 18, 2021
################################################################################

# Umfpack lib usually requires linking to a blas library.
# It is up to the user of this module to find a BLAS and link to it.

if (UMFPACK_INCLUDES AND UMFPACK_LIBRARIES)
  set(UMFPACK_FIND_QUIETLY TRUE)
endif ()

find_path(UMFPACK_INCLUDES
  NAMES
  umfpack.h
  PATHS
  $ENV{UMFPACKDIR}
  ${INCLUDE_INSTALL_DIR}
  PATH_SUFFIXES
  suitesparse
  ufsparse
)

find_library(UMFPACK_LIBRARIES umfpack PATHS $ENV{UMFPACKDIR} ${LIB_INSTALL_DIR})

if(UMFPACK_LIBRARIES AND NOT UMFPACK_LIBDIR)
    get_filename_component(UMFPACK_LIBDIR ${UMFPACK_LIBRARIES} PATH)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UMFPACK DEFAULT_MSG
                                  UMFPACK_INCLUDES UMFPACK_LIBRARIES UMFPACK_LIBDIR)

mark_as_advanced(UMFPACK_INCLUDES UMFPACK_LIBRARIES UMFPACK_LIBDIR)
