add_library(maloc_tmp STATIC vio.c vnm.c vmem.c vcom.c)
target_include_directories(maloc_tmp PUBLIC ${PROJECT_SOURCE_DIR}/src)
