message(STATUS "Building mesh")

set(LIBS "")
list(APPEND LIBS "apbs_generic")
list(APPEND LIBS "apbs_mg")
list(APPEND LIBS "apbs_pmgc")
if(FETK_ENABLED)
    list(APPEND LIBS "apbs_fem")
endif(FETK_ENABLED)

message(STATUS "libraries: ${LIBS}")

add_executable(mgmesh mgmesh.c)
target_link_libraries(mgmesh ${LIBS})

add_executable(smooth smooth.c)
target_link_libraries(smooth ${LIBS})

add_executable(dxmath dxmath.c)
target_link_libraries(dxmath ${LIBS})

add_executable(mergedx mergedx.c)
target_link_libraries(mergedx ${LIBS})

add_executable(mergedx2 mergedx2.c)
target_link_libraries(mergedx2 ${LIBS})

add_executable(value value.c)
target_link_libraries(value ${LIBS})

add_executable(multivalue multivalue.c)
target_link_libraries(multivalue ${LIBS})

add_executable(benchmark benchmark.c)
target_link_libraries(benchmark ${LIBS})

add_executable(similarity similarity.c)
target_link_libraries(similarity ${LIBS})

add_executable(analysis analysis.c)
target_link_libraries(analysis ${LIBS})

add_executable(dx2mol dx2mol.c)
target_link_libraries(dx2mol ${LIBS})

add_executable(dx2uhbd dx2uhbd.c)
target_link_libraries(dx2uhbd ${LIBS})

add_executable(del2dx del2dx.c)
target_link_libraries(del2dx ${LIBS})

add_executable(tensor2dx tensor2dx.c)
target_link_libraries(tensor2dx ${LIBS})

add_executable(uhbd_asc2bin uhbd_asc2bin.c)
target_link_libraries(uhbd_asc2bin ${LIBS})
