
#  From https://www.youtube.com/watch?v=y9kSr5enrSk (41 minutes into)

set(BOOST_VERSION 1.70)

#  NOTE: Should FALSE be changed to an OPTION for BUILD_SHARED_LIBS?
set(Boost_USE_STATIC_LIBS FALSE)
set(Boost_USE_MULTITHREADED TRUE)
set(Boost_USE_STATIC_RUNTIME FALSE)
set(Boost_COMPILER "-gcc8")

find_package(Boost ${BOOST_VERSION}
             EXACT
             CONFIG
             REQUIRED COMPONENTS headers
             )
#                                program_options
#             )

if(Boost_FOUND)
    #  NOTE: This sets all the Boost components as globally available
    #set_target_properties(Boost::headers
    #                      Boost::program_options
    set_target_properties(Boost::headers
                          PROPERTIES IMPORTED_GLOBAL TRUE)
                         )
endif()