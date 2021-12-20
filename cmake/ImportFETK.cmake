# ImportFETK.cmake
# Author: N.S. Oblath


macro(import_fetk FETK_IMPORT_VERSION)
    set(ENABLE_FETK TRUE) # FETK is required

    set(FETK_FROM_PKG TRUE)
    if(NOT FETK_IMPORT_VERSION MATCHES "[0-9]+\.[0-9]+\.[0-9]+")
        set(FETK_FROM_PKG FALSE)
        string(REPLACE "/" "-" FETK_IMPORT_VERSION ${FETK_IMPORT_VERSION}) # this matches the replacement that GitHub makes for the directory within the zip file
    endif()

    if(ENABLE_FETK)
    
    #    if(FETK_FROM_PKG)
        if(FALSE)

            set(FETK_BASE_URL "https://github.com/Electrostatics/FETK/releases/download/${FETK_IMPORT_VERSION}")
            if(WIN32)
                set(FETK_ZIP_EXT zip)
                set(FETK_SYSTEM_COMPONENT win32)
            else()
                set(FETK_ZIP_EXT tar.gz)
                set(FETK_SYSTEM_COMPONENT ${CMAKE_SYSTEM_NAME})
            endif()

            FetchContent_Declare( fetk
                URL ${FETK_BASE_URL}/FETK-${FETK_IMPORT_VERSION}-${FETK_SYSTEM_COMPONENT}.${FETK_ZIP_EXT}
            )
            FetchContent_MakeAvailable( fetk )

            list(APPEND CMAKE_MODULE_PATH ${fetk_SOURCE_DIR}/share/fetk/cmake)
            include_directories(${fetk_SOURCE_DIR}/include)
            link_directories(${fetk_SOURCE_DIR}/lib)

        else()

            FetchContent_Declare( fetk
                GIT_REPOSITORY https://github.com/Electrostatics/FETK.git
                GIT_TAG ${FETK_IMPORT_VERSION}
            )
            FetchContent_MakeAvailable( fetk )

            list(APPEND CMAKE_MODULE_PATH ${fetk_SOURCE_DIR}/cmake)
            include_directories(
                ${fetk_SOURCE_DIR}/maloc/src/base
                ${fetk_SOURCE_DIR}/maloc/src/psh
                ${fetk_SOURCE_DIR}/maloc/src/vsh
                ${fetk_SOURCE_DIR}/maloc/src/vsys
                ${fetk_SOURCE_DIR}/mc/src/aprx
                ${fetk_SOURCE_DIR}/mc/src/bam
                ${fetk_SOURCE_DIR}/mc/src/base
                ${fetk_SOURCE_DIR}/mc/src/dyn
                ${fetk_SOURCE_DIR}/mc/src/gem
                ${fetk_SOURCE_DIR}/mc/src/mcsh
                ${fetk_SOURCE_DIR}/mc/src/nam
                ${fetk_SOURCE_DIR}/mc/src/pde
                ${fetk_SOURCE_DIR}/mc/src/whb
                ${fetk_SOURCE_DIR}/punc/src/base
            )

        endif()

        list(APPEND APBS_LIBS
            maloc
            punc
            mc
            gamer
            superlu
            vf2c
        )

    #    find_package( BLAS REQUIRED )
    #    find_package( UMFPACK REQUIRED )
    #    list(APPEND APBS_LIBS
    #        ${UMFPACK_LIBRARIES}
    #        ${BLAS_LIBRARIES}
    #    )

    #    find_package( SuperLU )
    #    if(SuperLU_FOUND)
    #        # include from find_package variables
    #        list(APPEND APBS_LIBS
    #            ${SUPERLU_LIBRARIES}
    #        )
    #    else()
    #        # library built with FETK
    #        list(APPEND APBS_LIBS superlu)
    #    endif()

        SET(HAVE_MC 1)
        SET(HAVE_PUNC 1)
        SET(HAVE_GAMER 1)

    endif(ENABLE_FETK)

endmacro()
