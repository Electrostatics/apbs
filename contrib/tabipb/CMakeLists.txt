cmake_minimum_required (VERSION 2.8)
project (tabipb)
enable_language (Fortran)
OPTION (BUILD_SHARED_LIBS "Build shared libraries." OFF)

# make sure that the default is a RELEASE
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RELEASE CACHE STRING
      "Choose the type of build, options are: None Debug Release."
      FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# default installation
get_filename_component (default_prefix "." ABSOLUTE)
#set (CMAKE_INSTALL_PREFIX ${default_prefix} CACHE STRING
 #     "Choose the installation directory; by default it installs in the tabipb directory."
 #     FORCE)

# FFLAGS depend on the compiler
get_filename_component (Fortran_COMPILER_NAME ${CMAKE_Fortran_COMPILER} NAME)

if (Fortran_COMPILER_NAME MATCHES "gfortran.*")
  # gfortran
  set (CMAKE_Fortran_FLAGS_RELEASE "-funroll-all-loops -fno-f2c -O3")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-fno-f2c -O0 -g")
elseif (Fortran_COMPILER_NAME MATCHES "ifort.*")
  # ifort (untested)
  set (CMAKE_Fortran_FLAGS_RELEASE "-f77rtl -O3")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-f77rtl -O0 -g")
elseif (Fortran_COMPILER_NAME MATCHES "g77")
  # g77
  set (CMAKE_Fortran_FLAGS_RELEASE "-funroll-all-loops -fno-f2c -O3 -m32")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-fno-f2c -O0 -g -m32")
else (Fortran_COMPILER_NAME MATCHES "gfortran.*")
  message ("CMAKE_Fortran_COMPILER full path: " ${CMAKE_Fortran_COMPILER})
  message ("Fortran compiler: " ${Fortran_COMPILER_NAME})
  message ("No optimized Fortran compiler flags are known, we just try -O2...")
  set (CMAKE_Fortran_FLAGS_RELEASE "-O2")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-O0 -g")
endif (Fortran_COMPILER_NAME MATCHES "gfortran.*")


set (LIBFILES src/tabipb.f90 src/var_modules.f90 src/treecode3d_pb.f
src/readin.f90 src/dgmres_dep.f)
set (MODULES ${CMAKE_BINARY_DIR}/comdata.mod  ${CMAKE_BINARY_DIR}/molecule.mod
${CMAKE_BINARY_DIR}/treecode3d_procedures.mod  ${CMAKE_BINARY_DIR}/treecode.mod)

message(***** ${CMAKE_INSTALL_PREFIX})
#if(NOT DEFINED ${CMAKE_INSTALL_PREFIX})
#    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
#endif()
#if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
#endif()

set(TABIPB_LIBNAME tabipb)
if(BUILD_SHARED_LIBS)
    add_library(${TABIPB_LIBNAME} ${LIBFILES})
    install(TARGETS ${TABIPB_LIBNAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${TABIPB_LIBNAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    set(TABIPB_LIBNAME_STATIC "${TABIPB_LIBNAME}")
    add_library(${TABIPB_LIBNAME_STATIC} ${LIBFILES})
    install(TARGETS ${TABIPB_LIBNAME_STATIC}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${TABIPB_LIBNAME_STATIC}${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(FILES ${MODULES} DESTINATION ${CMAKE_INSTALL_LIBDIR})

