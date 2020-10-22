add_items(
    SOURCES
    vgrid.c
    vmgrid.c
    vopot.c
    vpmg.c
    vpmgp.c
)

add_items(
    EXTERNAL_HEADERS
    vgrid.h
    vmgrid.h
    vopot.h
    vpmg.h
    vpmgp.h
)

add_sublibrary(mg apbs_generic apbs_pmgc)

add_custom_command(
  TARGET apbs_mg
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:apbs_mg> "${PROJECT_SOURCE_DIR}/apbs/lib/$<TARGET_FILE_NAME:apbs_mg>"
  )
