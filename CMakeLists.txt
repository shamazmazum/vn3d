cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0048 NEW)

project (vn3d VERSION 0.6)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules")
set (CMAKE_C_FLAGS_RELEASE "-O3 -march=native")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
find_package (Dtrace)
find_package (Doxygen)
find_package (TurboJpeg)

option (LINEAR_INTERPOLATION "Faster linear interpolation" OFF)
add_subdirectory (src)

if (DOXYGEN_FOUND)
  add_subdirectory (doc)
endif (DOXYGEN_FOUND)

if (TURBOJPEG_FOUND)
  add_subdirectory (examples)
endif (TURBOJPEG_FOUND)
