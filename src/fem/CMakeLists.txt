add_items(
    SOURCES
    vcsm.c
    vfetk.c
    vpee.c
)

add_items(
    EXTERNAL_HEADERS
    vcsm.h
    vfetk.h
    vpee.h
)

add_sublibrary(fem)

add_custom_command(
  TARGET apbs_fem
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:apbs_fem> 
    ${PROJECT_SOURCE_DIR}/apbs/lib/$<TARGET_FILE_NAME:apbs_fem>
  )
