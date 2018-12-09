cmake_minimum_required (VERSION 2.6)

project (vn3d)

set (vn3d_VERSION_MAJOR 0)
set (vn3d_VERSION_MINOR 4)
set (vn3d_VERSION ${vn3d_VERSION_MAJOR}.${vn3d_VERSION_MINOR})

set (CMAKE_C_FLAGS_RELEASE "-O3 -march=native")

option (LINEAR_INTERPOLATION "Faster linear interpolation" OFF)
add_subdirectory (src)

find_package (Doxygen)
if (DOXYGEN_FOUND)
  add_subdirectory (doc)
endif (DOXYGEN_FOUND)
