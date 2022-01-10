cmake_minimum_required (VERSION 3.16)

###############################################################################
#  Get the Current Version Number so that it can be embedded in files later
###############################################################################
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/VERSION" _VERSION_DATA REGEX ^[0-9]+_[0-9]+_[0-9]+)
string(REPLACE "_" ";" _VERSION_LIST ${_VERSION_DATA})
list(GET _VERSION_LIST 0 MAJOR_VERSION)
list(GET _VERSION_LIST 1 MINOR_VERSION)
list(GET _VERSION_LIST 2 MICRO_VERSION)

set(CMAKE_PROJECT_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION})

################################################################################
# TODO:  Add licensing and authorship information
# TODO:  Test with FEtk build
# TODO:  Handle special mac dependencies
#        (configure.ac:1306)
################################################################################
#set(CMAKE_VERBOSE_MAKEFILE false)
cmake_policy(SET CMP0042 NEW) # MacOS RPATH on by default
cmake_policy(SET CMP0048 NEW) # project() command manages the VERSION variables
cmake_policy(SET CMP0054 NEW) # Only interpret if() arguments as variables or keywords when unquoted
cmake_policy(SET CMP0077 NEW) # option() honors normal variables (i.e. does nothing if a normal variable with the same name exists)
cmake_policy(SET CMP0091 NEW) # MSVC runtime library flags are selected by an abstraction (i.e. CMAKE_MSVC_RUNTIME_LIBRARY)
set(CMAKE_MACOSX_RPATH 1)

################################################################################
# Set up basic project stuff                                                   #
################################################################################
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(ExternalProject)
include(FetchContent)
include(FeatureSummary)
include(InstallRequiredSystemLibraries)

set(APBS_VERSION     "${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")
set(PACKAGE_STRING   "APBS ${APBS_VERSION}")
set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")

################################################################################
#  NOTE: Overwrite the RPATH information during the install from
#  https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
################################################################################
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#include(GNUInstallDirs)
#file(RELATIVE_PATH relDir
#     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
#     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
#)
#set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/${relDir})

project("APBS" VERSION ${APBS_VERSION})
set(CMAKE_PROJECT_DESCRIPTION "Adaptive Poisson-Boltzmann Solver")
set(CMAKE_PROJECT_HOMEPAGE_URL "http://www.poissonboltzmann.org/")

set_package_properties(APBS PROPERTIES
    URL "${CMAKE_PROJECT_HOMEPAGE_URL}"
    DESCRIPTION "${CMAKE_PROJECT_DESCRIPTION}"
    PURPOSE "APBS solves the equations of continuum electrostatics for large \
             biomolecular assemblages. This software was designed 'from the \
             ground up' using modern design principles to ensure its ability \
             to interface with other computational packages and evolve as \
             methods and applications change over time. The APBS code is \
             accompanied by extensive documentation for both users and \
             programmers and is supported by a variety of utilities for \
             preparing calculations and analyzing results. Finally, the free, \
             open-source APBS license ensures its accessibility to the entire \
             biomedical community.")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Toolchain file is set to <${CMAKE_TOOLCHAIN_FILE}>.  Including the file.")
    include(${CMAKE_TOOLCHAIN_FILE})
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Install prefix" FORCE)
endif()
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_INSTALL_DATAROOTDIR "share/apbs")
include(GNUInstallDirs)

set(APBS_ROOT ${PROJECT_SOURCE_DIR})
set(APBS_BUILD ${PROJECT_BINARY_DIR})
set(TOOLS_PATH ${APBS_BUILD}/tools)

set(APBS_LIBS)
set(APBS_LIB_DIRS)

find_file(CONTRIB_PATH "contrib"
          PATHS "${APBS_ROOT}"
          DOC "The path to contributed modules for apbs")
find_file(EXTERNALS_PATH "externals"
          PATHS "${APBS_ROOT}"
          DOC "The path to the external git submodules")

set(SYS_LIBPATHS /usr/lib64)
#list(APPEND SYS_LIBPATHS /usr/lib64)

# If this option is true: static libraries will be built
#                         static dependencies will be preferred
#                         executables will be statically linked
# If this option is false: dynamic libraries will be built
#                          dynamic libraries of dependencies will be preferred
#                          executables will be dynamically linked
option(APBS_STATIC_BUILD "Flag to indicate whether a static build should be used (if false, will be dynamic)" ON)

if(APBS_STATIC_BUILD)
    set(FETK_STATIC_BUILD ON)
    set(BLA_STATIC ON)
    set(BUILD_SHARED_LIBS OFF)
    #set(CMAKE_LINK_SEARCH_START_STATIC ON)
    #set(CMAKE_LINK_SEARCH_END_STATIC ON)
    if(WIN32)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
    else()
        list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .a)
    endif()
else()
    set(FETK_STATIC_BUILD OFF)
    set(BLA_STATIC OFF)
    set(BUILD_SHARED_LIBS ON)
    #set(CMAKE_LINK_SEARCH_START_STATIC OFF)
    #set(CMAKE_LINK_SEARCH_END_STATIC OFF)
    if(WIN32)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
        list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .dll)
    elseif(APPLE)
        list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .dylib .so)
    else()
        list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .so)
    endif()
endif()

message(STATUS "")
message(STATUS "APBS Static build: ${APBS_STATIC_BUILD}")
message(STATUS "Building shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "Find-library suffixes: ${CMAKE_FIND_LIBRARY_SUFFIXES}")
message(STATUS "")

################################################################################
# Handle the Finite Element Toolkit
#
# To use a released version of FETK:
#   - Set FETK_VERSION to the version number (e.g. 1.8.0)
# In this case, the binary archive will be downloaded and used.
#
# To use a non-released version of FETK:
#   - Set FETK_VERSION to the git branch or commit hash you want to use (e.g. main)
################################################################################

# FETK is currently required, and therefore this option is disabled
#option(ENABLE_FETK "Enable the finite element solver" ON)

set(FETK_VERSION "1.8.1" CACHE STRING "Version of FETK to use")
include(ImportFETK)
import_fetk(${FETK_VERSION})


###############################################################################
##  NOTE: For Debugging and generally nice information to have
###############################################################################
string(TIMESTAMP COMPILE_TIME)
string(TIMESTAMP COMPILE_YEAR "%Y")

message(STATUS "#############################################################")
message(STATUS "** PROJECT NAME:  ${CMAKE_PROJECT_NAME}")
message(STATUS "** PROJECT DESC:  ${CMAKE_PROJECT_DESCRIPTION}")
message(STATUS "** PROJECT URL:   ${CMAKE_PROJECT_HOMEPAGE_URL}")
message(STATUS "** VERSION_DATA:  ${CMAKE_PROJECT_VERSION}")
message(STATUS "** CURRENT_TIME:  ${COMPILE_TIME}")
message(STATUS "** CURRENT_YEAR:  ${COMPILE_YEAR}")


################################################################################
# Set project paths                                                            #
################################################################################
message(STATUS "Setting project paths")

set(CMAKE_CXX_STANDARD 17)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W4")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE /STACK:100000000")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -fpermissive -fPIC")
endif()
if(APPLE AND BUILD_SHARED_LIBS AND (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU"))
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -undefined dynamic_lookup")
endif()


###############################################################################
#####  Boilerplate Setup
###############################################################################
# include(GNUInstallDirs)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
# message(STATUS "** Project will be installed to ${CMAKE_INSTALL_PREFIX}")


###############################################################################
#####  Offer the builder the choice of overriding the installation directories
###############################################################################
# set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
# set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
# set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header files")
# set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")


###############################################################################
##### Report to builder
###############################################################################
# foreach(p LIB BIN INCLUDE CMAKE)
#   file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
#   message(STATUS "** Installing ${p} components to ${_path}")
#   unset(_path)
# endforeach()


################################################################################
# Debian/Ubuntu hack                                                           #
################################################################################
if(EXISTS "/etc/debian_version" AND
  "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
endif()


################################################################################
# Set up temporary files and directories                                       #
################################################################################
set(TEMP_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/temp)
file(MAKE_DIRECTORY ${TEMP_OUTPUT_DIR})


################################################################################
# Set the lookup paths for external library dependencies                       #
################################################################################
message(STATUS "Setting lookup paths for headers and libraries")

set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH}")
list(APPEND CMAKE_INCLUDE_PATH /usr/include)
list(APPEND CMAKE_INCLUDE_PATH /usr/local/include)


################################################################################
# Enable ansi pedantic compiling                                               #
################################################################################
option(ENABLE_PEDANTIC "Enable the pedantic ansi compilation" OFF)

if(ENABLE_PEDANTIC)
    ADD_DEFINITIONS("-Wall -pedantic -ansi")
    message(STATUS "Pedantic compilation enabled")
endif()


################################################################################
# Determine Machine Epsilon values                                             #
################################################################################
OPTION(CHECK_EPSILON "Compute machine epsilon values" YES)

if(CHECK_EPSILON)
    message(STATUS "Computing machine epsilon values")
    try_run(
        FLOAT_EPSILON_COMPILED
        FLOAT_EPSILON_COMPUTED
        ${TEMP_OUTPUT_DIR}
        ${APBS_ROOT}/src/.config/float_epsilon.c
        RUN_OUTPUT_VARIABLE FLOAT_EPSILON_OUTPUT
    )

    if(FLOAT_EPSILON_COMPUTED)
        message(STATUS "Floating point epsilon is ${FLOAT_EPSILON_OUTPUT}")
        set(FLOAT_EPSILON ${FLOAT_EPSILON_OUTPUT})
    else()
        message(FATAL_ERROR "Couldn't compute floating point machine epsilon")
    endif()

    try_run(
        DOUBLE_EPSILON_COMPILED
        DOUBLE_EPSILON_COMPUTED
        ${TEMP_OUTPUT_DIR}
        ${APBS_ROOT}/src/.config/double_epsilon.c
        RUN_OUTPUT_VARIABLE DOUBLE_EPSILON_OUTPUT
    )

    if(DOUBLE_EPSILON_COMPUTED)
        message(STATUS "Double precision epsilon is ${DOUBLE_EPSILON_OUTPUT}")
        set(DOUBLE_EPSILON ${DOUBLE_EPSILON_OUTPUT})
    else()
        message(FATAL_ERROR "Couldn't compute double precision machine epsilon")
    endif()
else()
  set(FLOAT_EPSILON "1.19209290e-7")
  set(DOUBLE_EPSILON "2.2204460492503131e-16")
endif()



################################################################################
# Check for a few required functions                                           #
################################################################################
CHECK_FUNCTION_EXISTS(time HAVE_TIME_FUNC)
if(NOT HAVE_TIME_FUNC)
    message(FATAL_ERROR "Required time function not found")
endif()


CHECK_FUNCTION_EXISTS(rand HAVE_RAND_FUNC)
if(NOT HAVE_RAND_FUNC)
    message(FATAL_ERROR "Required rand function not found")
endif()


CHECK_FUNCTION_EXISTS(srand HAVE_SRAND_FUNC)
if(NOT HAVE_SRAND_FUNC)
    message(FATAL_ERROR "Required srand function not found")
endif()


################################################################################
# Find some libraries                       #
################################################################################
if(NOT WIN32)
    find_library(MATH_LIBRARY "m")
    list(APPEND APBS_LIBS m stdc++)
endif()

find_package( UMFPACK REQUIRED )
find_package( SuiteSparse REQUIRED )
message(STATUS "UMFPack libraries: ${UMFPACK_LIBRARIES}")
message(STATUS "SUITESPARSE_INCLUDE_DIRS: ${SUITESPARSE_INCLUDE_DIRS}")
message(STATUS "SUITESPARSE_LIBRARIES: ${SUITESPARSE_LIBRARIES}")
list(APPEND APBS_LIBS
    ${UMFPACK_LIBRARIES}
    ${SUITESPARSE_LIBRARIES}
)

################################################################################
# Optionally copy NanoShaper executable                                        #
# the actual grab is later for BEM enable                                      #
################################################################################
option(GET_NanoShaper "Put NanoShaper executable in the build directory" OFF)

################################################################################
# Optionally build PYGBE: PYGBE method                                         #
################################################################################
option(ENABLE_PYGBE "Boundary element method using PYGBE" OFF)

if(ENABLE_PYGBE)
    message(STATUS "Building PYGBE")
    add_definitions(-DENABLE_PYGBE)
    message(STATUS "^^^^^ python min: ${PYTHON_MIN_VERSION}")
    message(STATUS "^^^^^ python max: ${PYTHON_MAX_VERSION}")
    if(PYTHON_MAX_VERSION)
        message(STATUS "**evaluates TRUE**")
    else()
        message(STATUS "**evaluates FALSE")
    endif()
    message(STATUS "cmake version: ${CMAKE_VERSION}")
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19")
        message(STATUS "  version >= 3.19")
    else()
        message(STATUS "  version < 3.19")
    endif()
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19" AND PYTHON_MAX_VERSION)
        message(STATUS "^^^^ finding python with min AND max")
        find_package(Python3 ${PYTHON_MIN_VERSION}...<${PYTHON_MAX_VERSION} REQUIRED COMPONENTS Interpreter Development)
    else()
    message(STATUS "^^^^ finding python with min ONLY")
    find_package(Python3 ${PYTHON_MIN_VERSION} REQUIRED COMPONENTS Interpreter Development)
        if(PYTHON_MAX_VERSION AND ${Python3_VERSION} VERSION_GREATER_EQUAL ${PYTHON_MAX_VERSION})
            set(Python3_FOUND FALSE)
            message(FATAL_ERROR "Python version ${Python3_VERSION} is too high; maximum is ${PYTHON_MAX_VERSION}")
        endif()
    endif()
    message(STATUS "******** PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS}")
    message(STATUS "******** PYTHON_LIBRARIES ${Python3_LIBRARIES}")
    include_directories(${Python3_INCLUDE_DIRS})
    list(APPEND APBS_LIBS ${Python3_LIBRARIES})

    # make sure we have PYGBE
    # detect virtualenv and set pip args accordingly
    if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
        set(_pip_args)
    else()
        set(_pip_args "--user")
    endif()
    # then install PYGBE with pip
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install pygbe ${_pip_args})
endif() # ENABLE_PYGBE


################################################################################
# Optionally build BEM: TABI-PB method                                         #
################################################################################
option(ENABLE_BEM "Boundary element method using TABIPB" OFF)

if(ENABLE_BEM)
    message(STATUS "Building TABIPB")
    #set(TABI bem)
    #set(TABI_PATH ${EXTERNALS_PATH}/${TABI})
    set(GET_NanoShaper ON) #CACHE BOOL "Grabbing NanoShaper path"
    add_definitions(-DENABLE_BEM)
    # We turn the TABIPB/APBS wrapper off and build here instead
    set(ENABLE_TABIPB_APBS OFF CACHE BOOL "Turn off TABIPB/APBS wrapper" FORCE)

    #add_subdirectory(${EXTERNALS_PATH}/${TABI})
    FetchContent_Declare( tabi
        GIT_REPOSITORY https://github.com/Treecodes/TABI-PB.git
        GIT_TAG fe1c237b057418fed48535db125394607040d9de
    )
    FetchContent_MakeAvailable( tabi )

    list(APPEND APBS_LIB_DIRS "${tabi_BINARY_DIR}/lib")
    list(APPEND APBS_LIBS TABIPBlib)
    include_directories(${tabi_SOURCE_DIR}/src)
    include_directories(${tabi_SOURCE_DIR}/src/tabipb_wrap)
    add_definitions(-DTABIPB_APBS)
  
    set(TABIPB_LIBFILES
        ${tabi_SOURCE_DIR}/src/params.cpp
        ${tabi_SOURCE_DIR}/src/molecule.cpp
        ${tabi_SOURCE_DIR}/src/particles.cpp
        ${tabi_SOURCE_DIR}/src/tree.cpp
        ${tabi_SOURCE_DIR}/src/clusters.cpp 
        ${tabi_SOURCE_DIR}/src/interaction_list.cpp
        ${tabi_SOURCE_DIR}/src/boundary_element.cpp 
        ${tabi_SOURCE_DIR}/src/gmres.cpp 
        ${tabi_SOURCE_DIR}/src/precondition.cpp 
        ${tabi_SOURCE_DIR}/src/output.cpp
        ${tabi_SOURCE_DIR}/src/tabipb_wrap/TABIPBWrap.cpp
        ${tabi_SOURCE_DIR}/src/tabipb_wrap/params_apbs_ctor.cpp
        ${tabi_SOURCE_DIR}/src/tabipb_wrap/molecule_apbs_ctor.cpp
    )

    set(TABIPB_HEADERS
        ${tabi_SOURCE_DIR}/src/boundary_element.h
        ${tabi_SOURCE_DIR}/src/clusters.h
        ${tabi_SOURCE_DIR}/src/constants.h
        ${tabi_SOURCE_DIR}/src/interaction_list.h
        ${tabi_SOURCE_DIR}/src/molecule.h
        ${tabi_SOURCE_DIR}/src/output.h
        ${tabi_SOURCE_DIR}/src/params.h
        ${tabi_SOURCE_DIR}/src/particles.h
        ${tabi_SOURCE_DIR}/src/partition.h
        ${tabi_SOURCE_DIR}/src/tabipb_timers.h
        ${tabi_SOURCE_DIR}/src/timer.h
        ${tabi_SOURCE_DIR}/src/tree.h
        ${tabi_SOURCE_DIR}/src/tabipb_wrap/TABIPBStruct.h
        ${tabi_SOURCE_DIR}/src/tabipb_wrap/TABIPBWrap.h)
  
    set(TABIPB_LIBNAME TABIPBlib)
    add_library(${TABIPB_LIBNAME} ${TABIPB_LIBFILES})
    target_compile_features(${TABIPB_LIBNAME} PRIVATE cxx_std_11)
    target_compile_options(${TABIPB_LIBNAME} PRIVATE 
                           $<$<CONFIG:RELEASE>:-O3>
                           $<$<CONFIG:RELWITHDEBINFO>:-O3>
                           $<$<CONFIG:DEBUG>:-O0 -Wall>)
    install(FILES ${TABIPB_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tabi COMPONENT headers)
    install(TARGETS ${TABIPB_LIBNAME} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
                       
endif() # ENABLE_BEM


################################################################################
# Getting nanoshaper binary
################################################################################
set(MESH_ROOT_URL_PATH "https://github.com/lwwilson1/mesh_routines/releases/download/v1.6")

if(GET_NanoShaper)
    set(NANOSHAPER_FULL_URL_PATH)
    set(NANOSHAPER_RAW_BINARY_NAME "NanoShaper")
    message(STATUS "Copying NanoShaper executable")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")      # Mac OS X specific code
        set(NANOSHAPER_FULL_URL_PATH "${MESH_ROOT_URL_PATH}/NanoShaper_OSX")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")   # Linux specific code
        set(NANOSHAPER_FULL_URL_PATH "${MESH_ROOT_URL_PATH}/NanoShaper_Linux64")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows") # Windows specific code
        set(NANOSHAPER_FULL_URL_PATH "${MESH_ROOT_URL_PATH}/NanoShaper32.exe")
        set(NANOSHAPER_RAW_BINARY_NAME "NanoShaper.exe")
    endif()

    if(NOT EXISTS "${TEMP_OUTPUT_DIR}/${NANOSHAPER_RAW_BINARY_NAME}")
        file(DOWNLOAD ${NANOSHAPER_FULL_URL_PATH} ${TEMP_OUTPUT_DIR}/${NANOSHAPER_RAW_BINARY_NAME})
        message(STATUS "NanoShaper Download: ${NANOSHAPER_FULL_URL_PATH} ${TEMP_OUTPUT_DIR}/${NANOSHAPER_RAW_BINARY_NAME}")
    endif()

    if(NOT ENABLE_BEM)
        install(PROGRAMS ${TEMP_OUTPUT_DIR}/${NANOSHAPER_RAW_BINARY_NAME}
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT executables)
        message(STATUS "NanoShaper install: ${TEMP_OUTPUT_DIR}/${NANOSHAPER_RAW_BINARY_NAME} ${CMAKE_INSTALL_BINDIR}")
    endif()
endif()


################################################################################
# Handle conditional building with MPI Support                                 #
# There may be a better way, but for now this comes before FETK because FETK   #
# depends on having the libraries located.                                     #
################################################################################
option(ENABLE_MPI "Enable MPI parallelism" OFF)

if(ENABLE_MPI)
    if(NOT ENABLE_DEBUG)
        message(STATUS "Checking for MPI")
        find_package(MPI)
        if(MPI_FOUND)
            message(STATUS "MPI support enabled")
            set(HAVE_MPI_H 1)
            include_directories(${MPI_C_INCLUDE_PATH})
            list(APPEND APBS_LIBS ${MPI_C_LIBRARIES})
        else()
            message(WARNING "MPI was not found; disabling")
        endif()
    else()
        message(WARNING "MPI may not be enabled in debugging mode")
    endif()
endif()


################################################################################
# Optionally build geoflow (git submodule)
################################################################################
option(ENABLE_GEOFLOW "geoflow method" OFF)

if(ENABLE_GEOFLOW)
    message(STATUS "Building GEOFLOW")
    #add_definitions(-DGEOFLOW_APBS)
    #set(ENABLE_GEOFLOW_APBS ON CACHE BOOL "Enable the APBS-Geoflow connection")

    #add_subdirectory(${EXTERNALS_PATH}/geoflow_c)
    FetchContent_Declare( geoflow
        GIT_REPOSITORY https://github.com/Electrostatics/geoflow_c.git
        GIT_TAG 39d53269c084f1dc1caa71de95dca77f19da739e
    )
    FetchContent_MakeAvailable( geoflow )

    add_definitions(-DENABLE_GEOFLOW)
    list(APPEND APBS_LIBS GeometricFlowLib)
    include_directories(${geoflow_SOURCE_DIR}/src)

    find_package(Eigen3 REQUIRED)
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()


################################################################################
# Optionally build pbam and pbsam (git submodule)
################################################################################
option(ENABLE_PBAM "pbam method" OFF)
option(ENABLE_PBSAM "pbsam method" OFF)

if(ENABLE_PBAM OR ENABLE_PBSAM)
    if(ENABLE_PBAM)
        message(STATUS "Building PBAM")
        set(ENABLE_PBAM_APBS ON CACHE BOOL "Enable the APBS-PBAM connection")

        add_definitions(
            -DENABLE_PBAM
            -DPBAM_APBS
        )
        list(APPEND APBS_LIBS PBAMLib)
    endif()

    if(ENABLE_PBSAM)
        message(STATUS "Building PBSAM")
        set(ENABLE_PBSAM_APBS ON CACHE BOOL "Enable the APBS-PBSAM connection")

        find_package(BLAS REQUIRED)

        add_definitions(
            -DENABLE_PBSAM
            -DPBSAM_APBS
        )
        list(APPEND APBS_LIBS PBSAMLib)
    endif()

    FetchContent_Declare( pb_s_am
        GIT_REPOSITORY https://github.com/Electrostatics/pb_solvers.git
        GIT_TAG 752f67704cdbbeb7c33c979758936d776024e832
    )
    #add_subdirectory(${EXTERNALS_PATH}/pb_s_am)
    FetchContent_MakeAvailable( pb_s_am )
endif()

if(ENABLE_PBAM)
    include_directories(${pb_s_am_SOURCE_DIR}/pbam/src)
endif()

if(ENABLE_PBSAM)
    include_directories(${pb_s_am_SOURCE_DIR}/pbam/src)
    if(ENABLE_PBAM)
        include(${pb_s_am_SOURCE_DIR}/cmake/Utils.cmake)
        include(${pb_s_am_SOURCE_DIR}/cmake/Dependencies.cmake)
        include(${pb_s_am_SOURCE_DIR}/cmake/Summary.cmake)
    endif()    
endif()

if(ENABLE_PBAM OR ENABLE_PBSAM)
    include_directories(${pb_s_am_SOURCE_DIR}/pb_shared/src)
    include_directories(${pb_s_am_SOURCE_DIR}/pb_wrap/src)
endif()


################################################################################
# Handle conditional fast mode                                                 #
################################################################################
option(ENABLE_FAST "Enable fast mode" OFF)

if(ENABLE_FAST)
    set(APBS_FAST 1)
    message(STATUS "Fast mode enabled")
endif()


################################################################################
# Handle conditional availability of macro embedding                           #
################################################################################
try_compile(
    HAVE_EMBED
    ${APBS_BUILD}/build
    ${APBS_ROOT}/src/.config/embed_test.c
)

# TODO: Determine if the EMBED macro is even used


################################################################################
# Handle conditional debug building                                            #
################################################################################
option(ENABLE_DEBUG "Enable debugging mode" OFF)

if(ENABLE_DEBUG)
    set(CMAKE_BUILD_TYPE "Debug")
    set(DEBUG 1)
    message(STATUS "Debugging compilation enabled")
endif()


################################################################################
# Enable inline functions conditionally dependent on debug mode                #
################################################################################
option(ENABLE_INLINE "Enable inline functions" ON)

if(ENABLE_INLINE)
    set(APBS_NOINLINE 1)
    message(STATUS "Inline functions enabled")
endif()


################################################################################
# Handle conditional building with quiet mode                                  #
################################################################################
option(ENABLE_QUIET "Enable quiet mode" OFF)

if(ENABLE_QUIET)
    set(VAPBSQUIET 1)
    message(STATUS "Quiet mode enabled")
endif()


################################################################################
# Handle conditional building with verbose debugging information printouts     #
################################################################################
option(ENABLE_VERBOSE_DEBUG "Enable verbose debugging mode" ON)

if(ENABLE_VERBOSE_DEBUG)
    set(VERBOSE_DEBUG 1)
    message(STATUS "Verbose debugging mode enabled")
endif()


################################################################################
# Configure Python                                                             #
# Buidling the Python goodness happens in the tool/python directory.           #
################################################################################
option(ENABLE_PYTHON "Enable python support" OFF)

#If(ENABLE_PYTHON AND UNIX AND NOT APPLE AND NOT BUILD_SHARED_LIBS)
#    message(FATAL_ERROR "In order to build the APBS Python shared library, BUILD_SHARED_LIBS must be enabled.")
#elseif(ENABLE_PYTHON AND APPLE AND BUILD_SHARED_LIBS)
#             message(FATAL_ERROR "ENABLE_PYTHON option in OSX systems requires the variable BUILD_SHARED_LIBS to be set to off.")
#endif()


################################################################################
# Handle conditional building with OpenMP                                      #
################################################################################
option(ENABLE_OPENMP "Enable OpenMP parallelism" OFF)

if(ENABLE_OPENMP)
    if(NOT ENABLE_DEBUG)
        message(STATUS "Checking for OpenMP")
        find_package(OpenMP)
        if(OPENMP_FOUND)
            message(STATUS "OpenMP support enabled")
            add_definitions("${OpenMP_C_FLAGS}")
            list(APPEND APBS_LIBS ${OpenMP_C_FLAGS})
        else()
            message(FATAL_ERROR "OpenMP was not found.  OpenMP support disabled")
        endif()
    else()
        message(WARNING "OpenMP may not be enabled in debugging mode")
    endif()
    if(NOT BUILD_SHARED_LIBS)
        message(FATAL_ERROR "OpenMP cannot be used with a static build")
    endif()
endif()


################################################################################
# Handle library checks for embedded unix environments in windows              #
################################################################################
if(MINGW)
    message(STATUS "Checking for wsock32 in MinGW environment")
    find_library(MINGW_WSOCK32
                 NAMES wsock32
                 PATHS ${SYS_LIBPATHS}
                 DOC   "The wsock32 library")

    if(MINGW_WSOCK32)
        message(STATUS "The wsock32 library was found: ${MINGW_HAS_WSOCK32}")
    else()
        message(FATAL_ERROR "The wsock32 library was not fond")
    endif()
endif()

if(CYGWIN)
    message(STATUS "Checking for wsock32 in Cygwin environment")
    find_library(CYGWIN_WSOCK32
                 NAMES wsock32
                 PATHS ${SYS_LIBPATHS}
                 DOC   "The wsock32 library")

    if(CYGWIN_WSOCK32)
        message(STATUS "The wsock32 library was found: ${CYGWIN_WSOCK32}")
        list(APPEND APBS_LIBS ${CYGWIN_WSOCK32})
    else()
        message(FATAL_ERROR "The wsock32 library was not fond")
    endif()

    set(HAVE_CYGWIN 1)
endif()

if(NOT CYGWIN AND NOT MINGW AND WIN32)
    list(APPEND APBS_LIBS wsock32 WS2_32)
    ADD_DEFINITIONS("/D _CRT_SECURE_NO_WARNINGS")
endif()


################################################################################
# Create APBS configuration file
################################################################################
configure_file(
    src/.config/apbscfg.h.in
    ${APBS_BUILD}/src/apbscfg.h
    @ONLY
)
if(ENABLE_iAPBS)
    INSTALL(FILES ${APBS_BUILD}/src/apbscfg.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers)
endif()


################################################################################
# Optionally build iAPBS interface                                             #
#   Note that this requires apbscfg.h,                                         #
#   and so must follow the APBS config file section                            #
################################################################################
option(ENABLE_iAPBS "Enable iAPBS" OFF)

if(ENABLE_iAPBS)
    message(STATUS "Building of iAPBS interface enabled")
    set(iAPBS_LIBRARY iapbs)
    include_directories(${APBS_BUILD}/src)
    add_subdirectory(contrib/iapbs/src)
    if(NOT TARGET ${iAPBS_LIBRARY})
        message(FATAL_ERROR "iAPBS target does not exist: <${iAPBS_LIBRARY}>")
    endif()
    list(APPEND APBS_LIBS ${iAPBS_LIBRARY})
else()
    message(STATUS "Building of iAPBS interface disabled")
endif()


################################################################################
# Build APBS sources                                                           #
################################################################################
link_directories(${APBS_LIB_DIRS})
include_directories(${APBS_ROOT}/src ${APBS_BUILD}/src ${APBS_ROOT}/include)
add_subdirectory(src)


################################################################################
# Build APBS documentation                                                     #
################################################################################
option(BUILD_DOC "Build/Rebuild documentation" ON)

if(BUILD_DOC)
    message(STATUS "Building documentation enabled")
    add_subdirectory(docs)
endif()


################################################################################
# Handle conditional building with verbose debugging information printouts     #
################################################################################
option(BUILD_TOOLS "Build supplemental tools" ON)

if(BUILD_TOOLS)
    message(STATUS "Supplemental tools enabled")
    add_subdirectory(tools)
endif()


################################################################################
# Set up additional directories to install                                     #
################################################################################
install(DIRECTORY ${APBS_ROOT}/docs
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        COMPONENT docs
        PATTERN "programmer" EXCLUDE
        PATTERN "CMakeLists.txt" EXCLUDE)

install(DIRECTORY ${APBS_ROOT}/examples
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        USE_SOURCE_PERMISSIONS
        COMPONENT examples)

install(DIRECTORY ${APBS_ROOT}/tests
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        COMPONENT tests
        FILES_MATCHING
        PATTERN "*.py"
        PATTERN "*.cfg"
        PATTERN "README.*")

if(BUILD_TOOLS)
    install(DIRECTORY ${APBS_ROOT}/tools
            DESTINATION ${CMAKE_INSTALL_DATADIR}
            USE_SOURCE_PERMISSIONS
            COMPONENT tools
            PATTERN "CMakeLists.txt"
            PATTERN "matlab"      EXCLUDE
            PATTERN "__pycache__" EXCLUDE
            PATTERN "*.py[co]"    EXCLUDE)

    if(NOT "${APBS_ROOT}" STREQUAL "${APBS_BUILD}")
    install(DIRECTORY ${APBS_BUILD}/tools/bin
            DESTINATION ${CMAKE_INSTALL_DATADIR}/tools
            USE_SOURCE_PERMISSIONS
            COMPONENT tools)
    endif()
endif()


###############################################################################
#####  Do Testing
###############################################################################
option(ENABLE_TESTS "Enable tests" ON)
if(${ENABLE_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()

###############################################################################
#####  Generate Packages
###############################################################################
# string(TOLOWER ${PROJECT_NAME}            PACKAGE_NAME)
set(PACKAGE_NAME                       "${PROJECT_NAME}")
set(CPACK_PACKAGE_NAME                 "${PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION          "APBS - Adaptive Poisson Boltzmann Solver")

set(CPACK_RESOURCE_FILE_LICENSE        "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_DESCRIPTION_FILE     "${PROJECT_SOURCE_DIR}/README.md")

set(CPACK_VERBATIM_VARIABLES            YES)
set(CPACK_SOURCE_IGNORE_FILES           ${PROJECT_BINARY_DIR}
                                        .gitignore
                                        /.git/
                                        /tools/matlab/
                                        io.mc
                                        \\.dx
                                        \\.err
                                        \\.log
                                        \\.out
                                        \\.vtk
)
set(CPACK_PACKAGE_VERSION_MAJOR        "${CMAKE_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR        "${CMAKE_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH        "${CMAKE_MICRO_VERSION}")
#  NOTE: The following is tempting but does not work!
# set(FULL_PACKAGE_NAME                  "${PACKAGE_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")
set(CPACK_PACKAGE_VENDOR               "PNNL")
set(CPACK_PACKAGE_CONTACT              "APBS - https://github.com/Electrostatics/apbs")
set(CPACK_PACKAGE_FILE_NAME            "${PACKAGE_NAME}-${APBS_VERSION}.${CMAKE_HOST_SYSTEM_NAME}")
set(CPACK_STRIP_FILES                   True)

list(APPEND CPACK_GENERATOR            "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY    True)

# Control which components get included in the package
# Components in use are: executables, libraries, headers, docs, examples, tests, tools, python
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
#list(REMOVE_ITEM CPACK_COMPONENTS_ALL "libraries" "headers" "docs" )
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "docs" )

if(CPACK_GENERATOR MATCHES "^(7Z|TBZ2|TGZ|TXZ|TZ|TZST|ZIP)$")
    set(CPACK_TOPLEVEL_TAG ${CPACK_PACKAGE_NAME})
    set(CPACK_SET_DESTDIR  False)
    set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY True)
endif()

include (CPack)
