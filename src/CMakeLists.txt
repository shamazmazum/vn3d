if (LINEAR_INTERPOLATION)
  add_definitions (-DLINEAR_INTERPOLATE)
endif (LINEAR_INTERPOLATION)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vn3d.ld.in ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld)
add_library (vn3d SHARED generic.c value.c worley.c)
set_target_properties (vn3d PROPERTIES VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld
  LINK_FLAGS "-Wl,--version-script ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld ${ADDITIONAL_LINK_FLAGS}")

install (TARGETS vn3d LIBRARY DESTINATION lib)
install (FILES vn3d.h generic.h value.h worley.h DESTINATION include/vn3d)
