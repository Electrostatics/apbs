
macro(apbs_find_python_libraries)
  set(CMAKE_FIND_FRAMEWORK NEVER)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  if(WIN32 OR WIN64)
    find_package(Python3 COMPONENTS Development REQUIRED)
  else()
    # We really only need the headers, but development also pulls in the
    # libraries. In some cases, this could cause the build to fail where it
    # should not, as when the headers are available but the development libs
    # are not. On windows however, the build will try to statically link 
    # against python.lib which is fine.
    find_package(Python3 COMPONENTS Development)
  endif()

  message(STATUS "***** Python3 include path is: ${Python3_INCLUDE_DIRS}")
  message(STATUS "***** Python3 library path is: ${Python3_LIBRARIES}")
  message(STATUS "***** Python3 library dir  is: ${Python3_LIBRARY_DIRS}")

  # There is a multitude of ways to discover the correct python libraries, so we
  # try to find them canonically and via the sys environment
  if(Python3_INCLUDE_DIRS)
    include_directories(${Python3_INCLUDE_DIRS})
  elseif($ENV{Python3_INCLUDE_DIRS})
    include_directories($ENV{Python3_INCLUDE_DIRS})
    set(Python3_INCLUDE_DIRS $ENV{Python3_INCLUDE_DIRS})
  elseif(Python_INCLUDE_DIRS)
    include_directories(${Python_INCLUDE_DIRS})
    set(Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
  else()
    message(FATAL_ERROR "Could not find python include path.")
  endif()
endmacro()

macro(apbs_find_pybind11_dep)
  find_package(pybind11)
  add_library(pybind11_dep INTERFACE)
  if(NOT TARGET pybind11::module)
    target_include_directories(pybind11_dep INTERFACE
      ${PROJECT_SOURCE_DIR}/externals/pybind11/include)
    include(${PROJECT_SOURCE_DIR}/externals/pybind11/tools/FindPythonLibsNew.cmake)
  else()
    target_link_libraries(pybind11_dep INTERFACE pybind11::module)
  endif()

  target_include_directories(pybind11_dep INTERFACE ${Python3_INCLUDE_DIRS} $ENV{Python3_INCLUDE_DIRS})
  if(WIN32 OR WIN64)
    target_link_libraries(pybind11_dep INTERFACE ${Python3_LIBRARIES})
  endif()
endmacro()

apbs_find_python_libraries()
apbs_find_pybind11_dep()
