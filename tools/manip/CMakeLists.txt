message(STATUS "Building manip")

set(LIBS "")
list(APPEND LIBS "apbs_generic")
list(APPEND LIBS "apbs_mg")
list(APPEND LIBS "apbs_pmgc")
if(FETK_ENABLED)
    list(APPEND LIBS "apbs_fem")
endif(FETK_ENABLED)

message(STATUS "libraries: ${LIBS}")

add_executable(coulomb coulomb.c)
target_link_libraries(coulomb ${LIBS})

add_executable(born born.c)
target_link_libraries(born ${LIBS})

foreach(BINARY born coulomb)
  add_custom_command(
    TARGET ${BINARY}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy 
      $<TARGET_FILE:${BINARY}>
      ${PROJECT_SOURCE_DIR}/apbs/bin/$<TARGET_FILE_NAME:${BINARY}>
    )
endforeach()
