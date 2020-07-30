cmake_minimum_required(VERSION 3.12)


set(CMAKE_PROJECT_INCLUDE_BEFORE
    "${CMAKE_CURRENT_LIST_DIR}/CMakeProjectInfo.in")

project(abps_pdb2pqr
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        HOMEPAGE_URL ${project_homepage}
        LANGUAGES C CXX)

###############################################################################
##  From:
##    https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
##
##  Use git to add, initialize, and update submodules for the user
###############################################################################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "** Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

###############################################################################
##  NOTE: For Debugging and generally nice information to have
###############################################################################
string(TIMESTAMP COMPILE_TIME)
message(STATUS "#############################################################")
message(STATUS "** PROJECT NAME:            ${CMAKE_PROJECT_NAME}")
message(STATUS "** PROJECT DESC:            ${CMAKE_PROJECT_DESCRIPTION}")
message(STATUS "** PROJECT URL:             ${CMAKE_PROJECT_HOMEPAGE_URL}")
message(STATUS "** VERSION_DATA:            ${CMAKE_PROJECT_VERSION}")
message(STATUS "** CURRENT_TIME:            ${COMPILE_TIME}")

###############################################################################
##  NOTE: For Testing
###############################################################################
option(ENABLE_TESTS "Enable tests" ON)

if (${ENABLE_TESTS})
    message(STATUS "** TESTS:                   Will be built")
    enable_testing()
endif()
message(STATUS "#############################################################")

###############################################################################
##  NOTE: Build APBS and PDB2PQR
###############################################################################
add_subdirectory(apbs)
add_subdirectory(pdb2pqr)
