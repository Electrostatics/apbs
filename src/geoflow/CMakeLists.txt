add_items(
    SOURCES
    cpbconcz2.cpp
    modules.cpp
    pbsolvercz.cpp
    solvationsub.cpp
    subfunction.cpp
    surfconcz.cpp
)

add_items(
    EXTERNAL_HEADERS
    cpbconcz2.h
)

add_sublibrary(geoflow)

# Enable C++11 features
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
		OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_definitions(-std=c++0x)
endif()
