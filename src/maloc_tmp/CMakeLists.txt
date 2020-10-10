add_library(maloc_tmp STATIC vio.cpp vnm.c vmem.c vcom.c)
target_include_directories(maloc_tmp 
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src)
