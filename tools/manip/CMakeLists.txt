message(STATUS "Building manip")

set( LIBS 
    apbs_generic
    apbs_mg
    apbs_pmgc
    apbs_fem
    apbs_routines
)

message(STATUS "libraries: ${LIBS}")

add_executable(coulomb coulomb.c)
target_link_libraries(coulomb ${LIBS})

add_executable(born born.c)
target_link_libraries(born ${LIBS})
