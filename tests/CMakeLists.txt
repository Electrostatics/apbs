include (CTest)
enable_testing()

message(STATUS "******* TESTING ADDED *****************")
#set (Python3_FIND_ABI "OFF" "ANY" "ANY")
find_package(Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter)

set(APBS_BINARY "apbs")
if(CMAKE_HOST_WIN32)
  set(APBS_BINARY "apbs.exe")
endif()

set(TESTS_CFG_FILE ${CMAKE_SOURCE_DIR}/tests/test_cases.cfg)

add_test(NAME atom_potentials_test
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test_atom_potentials.py $<TARGET_FILE:apbs>)
set_tests_properties(atom_potentials_test PROPERTIES TIMEOUT 180 LABELS regression)

add_test(NAME born_regression_test
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test_born_regression.py $<TARGET_FILE:apbs>)
set_tests_properties(born_regression_test PROPERTIES TIMEOUT 300 LABELS regression)

if(EXISTS ${TESTS_CFG_FILE})
    file(READ ${TESTS_CFG_FILE} TEST_CASES_FILE)
    string(REPLACE "\n" ";" TEST_CASES_LINES ${TEST_CASES_FILE}) 
    foreach(_line ${TEST_CASES_LINES})
        if(${_line} MATCHES "^\\[")
            string(REPLACE "[" "" _line ${_line})
            string(REPLACE "]" "" _line ${_line})
            string(REGEX REPLACE "[-].*$" "" _shortname ${_line})
            string(TOUPPER "ENABLE_${_shortname}" OPTION_NAME)
            message(STATUS "******* FOUND TEST CASE ${_shortname}, ${_line}")
            if (DEFINED "${OPTION_NAME}" AND NOT ${OPTION_NAME})
                message(STATUS "******* SKIPPING TEST CASE ${_shortname}, ${_line}")
            else()
                message(STATUS "TEST  ${_line}_test  WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/tests  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/apbs_tester.py -e ${APBS_BINARY} -t ${_line}")
                add_test(NAME        "${_line}_test" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tests" COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/apbs_tester.py -e ${APBS_BINARY} -t ${_line})
            endif()
        endif()
    endforeach()
else()
    message(FATAL  "******* MISSING Test Config (${TESTS_CFG_FILE})")
endif()

add_executable(vacc_surf_test vacc_surf_test.c)
target_link_libraries(vacc_surf_test
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
    ${APBS_LIBS}
)
if(APPLE)
    target_link_libraries(vacc_surf_test "-framework CoreFoundation")
endif()
add_test(NAME vacc_surf_test COMMAND $<TARGET_FILE:vacc_surf_test>)
set_tests_properties(vacc_surf_test PROPERTIES TIMEOUT 60 LABELS unit)

add_executable(pmg_trilin_test pmg_trilin_test.c)
target_link_libraries(pmg_trilin_test
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
    ${APBS_LIBS}
)
if(APPLE)
    target_link_libraries(pmg_trilin_test "-framework CoreFoundation")
endif()
add_test(NAME pmg_trilin_test COMMAND $<TARGET_FILE:pmg_trilin_test>)
set_tests_properties(pmg_trilin_test PROPERTIES TIMEOUT 60 LABELS unit)

add_executable(vpbe_reuse_test vpbe_reuse_test.c)
target_link_libraries(vpbe_reuse_test
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
    ${APBS_LIBS}
)
if(APPLE)
    target_link_libraries(vpbe_reuse_test "-framework CoreFoundation")
endif()
add_test(NAME vpbe_reuse_test COMMAND $<TARGET_FILE:vpbe_reuse_test>)
set_tests_properties(vpbe_reuse_test PROPERTIES TIMEOUT 60 LABELS unit)

add_executable(vpmg_fill_test vpmg_fill_test.c)
target_link_libraries(vpmg_fill_test
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
    ${APBS_LIBS}
)
if(APPLE)
    target_link_libraries(vpmg_fill_test "-framework CoreFoundation")
endif()
add_test(NAME vpmg_fill_test COMMAND $<TARGET_FILE:vpmg_fill_test>)
set_tests_properties(vpmg_fill_test PROPERTIES TIMEOUT 60 LABELS unit)

add_executable(vgrid_dx_test vgrid_dx_test.c)
target_link_libraries(vgrid_dx_test
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
    ${APBS_LIBS}
)
if(APPLE)
    target_link_libraries(vgrid_dx_test "-framework CoreFoundation")
endif()
add_test(NAME vgrid_dx_test COMMAND $<TARGET_FILE:vgrid_dx_test>)
set_tests_properties(vgrid_dx_test PROPERTIES TIMEOUT 60 LABELS unit)
