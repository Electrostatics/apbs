message(STATUS "Building manip")

set(LIBS "")
list(APPEND LIBS "apbs_generic")
list(APPEND LIBS "apbs_mg")
list(APPEND LIBS "apbs_pmgc")
if(FETK_ENALBED)
    list(APPEND LIBS "apbs_fem")
endif(FETK_ENALBED)

message(STATUS "libraries: ${LIBS}")

add_executable(coulomb coulomb.c)
target_link_libraries(coulomb ${LIBS})

add_executable(born born.c)
target_link_libraries(born ${LIBS})
