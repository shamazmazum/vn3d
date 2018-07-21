cmake_minimum_required (VERSION 2.6)

project (vn3d)

set (vn3d_VERSION_MAJOR 0)
set (vn3d_VERSION_MINOR 2)
set (vn3d_VERSION ${vn3d_VERSION_MAJOR}.${vn3d_VERSION_MINOR})

set (CMAKE_C_FLAGS_RELEASE "-O3 -march=native")

option (LINEAR_INTERPOLATION "Faster linear interpolation" OFF)
if (LINEAR_INTERPOLATION)
  add_definitions (-DLINEAR_INTERPOLATE)
endif (LINEAR_INTERPOLATION)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vn3d.ld.in ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld)
add_library (vn3d SHARED vn3d.c)
set_target_properties (vn3d PROPERTIES VERSION ${vn3d_VERSION}
  SOVERSION ${vn3d_VERSION_MAJOR}
  LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld
  LINK_FLAGS "-Wl,--version-script ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld ${ADDITIONAL_LINK_FLAGS}")

install (TARGETS vn3d LIBRARY DESTINATION lib)
install (FILES vn3d.h DESTINATION include/vn3d)
