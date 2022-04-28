# ImportFETK.cmake
# Author: N.S. Oblath


macro(import_fetk FETK_IMPORT_VERSION)
    set(ENABLE_FETK TRUE) # FETK is required

    set(FETK_FROM_PKG TRUE)
    set(MATCH_STRING "^v?[0-9]+\.[0-9]+\.[0-9]+$")
    if(NOT ${FETK_IMPORT_VERSION} MATCHES ${MATCH_STRING})
        set(FETK_FROM_PKG FALSE)
        string(REPLACE "/" "-" FETK_IMPORT_VERSION ${FETK_IMPORT_VERSION}) # this matches the replacement that GitHub makes for the directory within the zip file
    endif()

    if(ENABLE_FETK)
    
        if(FETK_FROM_PKG)

            set(FETK_BASE_URL "https://github.com/Electrostatics/FETK/releases/download/${FETK_IMPORT_VERSION}")
            if(WIN32)
                set(FETK_ZIP_EXT zip)
                set(FETK_SYSTEM_COMPONENT win32)
            else()
                set(FETK_ZIP_EXT tar.gz)
                set(FETK_SYSTEM_COMPONENT ${CMAKE_SYSTEM_NAME})
            endif()

            message(STATUS "Downloading built FETK package ${FETK_IMPORT_VERSION}")
            FetchContent_Declare( fetk
                URL ${FETK_BASE_URL}/FETK-${FETK_IMPORT_VERSION}-${FETK_SYSTEM_COMPONENT}.${FETK_ZIP_EXT}
            )
            FetchContent_MakeAvailable( fetk )

            list(APPEND CMAKE_MODULE_PATH ${fetk_SOURCE_DIR}/share/fetk/cmake)
            include_directories(${fetk_SOURCE_DIR}/include)
            link_directories(${fetk_SOURCE_DIR}/lib)

        else()

            # PMG is turned off because of some missing symbols: dc_vec__, dc_scal__, rand_, c_vec__, tsecnd_, and c_scal__
            set(BUILD_PMG OFF)
        
            message(STATUS "Building FETK from commit ${FETK_IMPORT_VERSION}")
            FetchContent_Declare( fetk
                GIT_REPOSITORY https://github.com/Electrostatics/FETK.git
                GIT_TAG ${FETK_IMPORT_VERSION}
            )
            FetchContent_MakeAvailable( fetk )

            list(APPEND CMAKE_MODULE_PATH ${fetk_SOURCE_DIR}/cmake)

        endif()

        # Only need to link to mc because mc depends on the others
        list(APPEND APBS_LIBS
            mc
        )

        SET(HAVE_MC 1)
        SET(HAVE_PUNC 1)
        SET(HAVE_GAMER 1)

    endif(ENABLE_FETK)

endmacro()
