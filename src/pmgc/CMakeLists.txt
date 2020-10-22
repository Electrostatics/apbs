add_items(
    SOURCES
    buildAd.c
    buildBd.c
    buildGd.c
    buildPd.c
    cgd.c
    gsd.c
    matvecd.c
    mgcsd.c
    mgdrvd.c
    mgsubd.c
    mikpckd.c
    mlinpckd.c
    mypdec.c
    newtond.c
    newdrvd.c
    powerd.c
    smoothd.c
    mgfasd.c
)

add_items(
    EXTERNAL_HEADERS
    buildAd.h
    buildBd.h
    buildGd.h
    buildPd.h
    cgd.h
    gsd.h
    matvecd.h
    mgcsd.h
    mgdrvd.h
    mgsubd.h
    mikpckd.h
    mlinpckd.h
    mypdec.h
    newtond.h
    newdrvd.h
    powerd.h
    smoothd.h
    mgfasd.h
)

add_sublibrary(pmgc)

add_custom_command(
  TARGET apbs_pmgc
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:apbs_pmgc> "${PROJECT_SOURCE_DIR}/apbs/lib/$<TARGET_FILE_NAME:apbs_pmgc>"
  )
