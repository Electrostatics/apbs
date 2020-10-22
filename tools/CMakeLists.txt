set(EXECUTABLE_OUTPUT_PATH ${TOOLS_PATH}/bin)

add_subdirectory(mesh)
add_subdirectory(manip)

if(ENABLE_PYTHON)
  set(CMAKE_FIND_FRAMEWORK NEVER)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  if(WIN32 OR WIN64)
    find_package(Python3 COMPONENTS Development REQUIRED)
  endif()

  INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
  message(STATUS "***** Python3 include path is: ${Python3_INCLUDE_DIRS}")
  message(STATUS "***** Python3 library path is: ${Python3_LIBRARIES}")
  message(STATUS "***** Python3 library dir  is: ${Python3_LIBRARY_DIRS}")

  include_directories(${Python3_INCLUDE_DIRS} $ENV{Python3_INCLUDE_DIRS})
  add_subdirectory(python)
  add_subdirectory(python_pybind)
endif(ENABLE_PYTHON)

