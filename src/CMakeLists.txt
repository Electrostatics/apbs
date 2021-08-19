cmake_minimum_required(VERSION 3.8)

macro(add_items TYPE)

    # Make sure the TYPE argument is valid
    set(ALLOWABLE_TYPES SOURCES EXTERNAL_HEADERS INTERNAL_HEADERS)
    list(FIND ALLOWABLE_TYPES ${TYPE} VALID_TYPE)
    if(VALID_TYPE EQUAL -1)
        message(
            FATAL_ERROR
            "Passed an invalid type to add_items macro.  "
            "Must be one of SOURCES, EXTERNAL_HEADERS, or INTERNAL_HEADERS.  "
            "Was ${TYPE}"
        )
    endif()

    # The items to add will be any additional parameters beyond TYPE
    set(ITEMS ${ARGN})

    # Add each item successively to the appropriately typed list
    foreach(ITEM ${ITEMS})
        list(APPEND ${TYPE} "${ITEM}")
    endforeach()

endmacro()

macro(add_sublibrary LIBRARY)
    set(LIBRARY_NAME "apbs_${LIBRARY}")
    set(LIBRARY_DEPENDENCIES ${APBS_LIBS} ${ARGN})
    message(STATUS "Adding ${LIBRARY_NAME}")
    message(STATUS "With source files ${SOURCES}")
    message(STATUS "With external header files ${EXTERNAL_HEADERS}")
    message(STATUS "With internal header files ${INTERNAL_HEADERS}")
    message(STATUS "With library dependencies ${LIBRARY_DEPENDENCIES}")
    add_library(${LIBRARY_NAME} ${SOURCES} ${EXTERNAL_HEADERS} ${INTERNAL_HEADERS})
    target_link_libraries(${LIBRARY_NAME} ${LIBRARY_DEPENDENCIES})
    #add_dependencies(${LIBRARY_NAME} ${LIBRARY_DEPENDENCIES})
    INSTALL(FILES ${EXTERNAL_HEADERS} DESTINATION ${HEADER_INSTALL_PATH}/${LIBRARY})
    INSTALL(TARGETS ${LIBRARY_NAME} DESTINATION ${LIBRARY_INSTALL_PATH})
    list(APPEND APBS_INTERNAL_LIBS ${LIBRARY_NAME})
    set(APBS_INTERNAL_LIBS ${APBS_INTERNAL_LIBS} PARENT_SCOPE)
    message(STATUS "Added ${LIBRARY_NAME}")
endmacro()

add_subdirectory(generic)
add_subdirectory(pmgc)
add_subdirectory(mg)

if(ENABLE_FETK)
   add_subdirectory(fem)
endif(ENABLE_FETK)

add_items(SOURCES routines.c)
add_sublibrary(routines)

message(STATUS "External Headers: ${EXTERNAL_HEADERS}")

#add_executable(apbs main.c routines.c)
add_executable(apbs main.c apbs.h routines.c routines.h)
message(STATUS " ")
message(STATUS "APBS Libraries: ${APBS_LIBS}")
message(STATUS "Internal Libraries: ${APBS_INTERNAL_LIBS}")
message(STATUS " ")

#add_dependencies(apbs ${BEM_LIB})
#target_include_directories(apbs PRIVATE apbs.h routines.h)
target_link_libraries(apbs ${APBS_LIBS} ${APBS_INTERNAL_LIBS} ${APBS_LIBS})
#add_dependencies(apbs ${APBS_LIBS} ${APBS_INTERNAL_LIBS} ${APBS_LIBS})

#configure_file(
#    ${APBS_ROOT}/src/.config/apbscfg.h.in
#    ${APBS_BUILD}/src/apbscfg.h
#    )

if(ENABLE_iAPBS)
#    ADD_LIBRARY(apbs_routines routines.c routines.h)
#    INSTALL(TARGETS apbs_routines DESTINATION ${LIBRARY_INSTALL_PATH})
    INSTALL(FILES ${APBS_BUILD}/src/apbscfg.h DESTINATION ${HEADER_INSTALL_PATH})
endif()


################################################################################
#  We have to fix the dynamically linked libraries 
################################################################################
if(APPLE)
  set(HARDPATH "${PROJECT_BINARY_DIR}/fetk/lib")
  foreach (hardlib ${APBS_LIBS})
    set(HARDNAME "lib${hardlib}.1.dylib")
    message(STATUS "#############################################################")
    message(STATUS "#############################################################")
    message(STATUS "#############################################################")
    message(STATUS "#############################################################")
    message(STATUS "** Change path for hardlib - ${hardlib}")
    message(STATUS "COMMAND install_name_tool -change ${HARDPATH}/${HARDNAME} @executable_path/../lib/${HARDNAME} ${PROJECT_BINARY_DIR}/bin/apbs")
    message(STATUS "#############################################################")
    add_custom_command(TARGET apbs POST_BUILD
                       COMMAND install_name_tool -change ${HARDPATH}/${HARDNAME} @executable_path/../lib/${HARDNAME} ${PROJECT_BINARY_DIR}/bin/apbs
    )
  endforeach()
endif()

INSTALL(FILES apbs.h routines.h DESTINATION ${HEADER_INSTALL_PATH})
INSTALL(TARGETS apbs DESTINATION ${EXECUTABLE_INSTALL_PATH})

message(STATUS ${CMAKE_C_FLAGS})
