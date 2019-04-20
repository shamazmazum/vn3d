cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0048 NEW)

project (vn3d VERSION 0.5)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules")
set (CMAKE_C_FLAGS_RELEASE "-O3 -march=native")
find_package (Dtrace)
find_package (Doxygen)

option (LINEAR_INTERPOLATION "Faster linear interpolation" OFF)
add_subdirectory (src)

if (DOXYGEN_FOUND)
  add_subdirectory (doc)
endif (DOXYGEN_FOUND)
