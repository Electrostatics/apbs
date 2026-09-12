add_items(
    SOURCES
    geoflow_wrap_apbs.cpp
)

add_items(
    EXTERNAL_HEADERS
    geoflow_wrap_apbs.h
)

add_sublibrary(geoflow_apbs)

if(TARGET eigen3)
    add_dependencies(apbs_geoflow_apbs eigen3)
    target_include_directories(apbs_geoflow_apbs PRIVATE
        ${geoflow_BINARY_DIR}/eigen3-prefix/src/eigen3)
elseif(TARGET Eigen3::Eigen)
    target_link_libraries(apbs_geoflow_apbs Eigen3::Eigen)
endif()
