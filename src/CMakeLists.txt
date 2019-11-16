if (LINEAR_INTERPOLATION)
  add_definitions (-DLINEAR_INTERPOLATE)
endif (LINEAR_INTERPOLATION)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vn3d.ld.in ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld)
add_library (vn3d SHARED generic.c value.c worley.c)

if (DTRACE_FOUND)
  add_definitions (-DWITH_DTRACE)
  include_directories (${CMAKE_CURRENT_BINARY_DIR})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dtrace.d
    ${CMAKE_CURRENT_BINARY_DIR}/dtrace.d COPYONLY)

  add_custom_command (
    OUTPUT  dtrace.h
    COMMAND ${DTRACE_EXECUTABLE} -h -s dtrace.d -o dtrace.h
    DEPENDS dtrace.d
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target (vn3d-dtrace-header ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dtrace.h)
  add_dependencies (vn3d vn3d-dtrace-header)

  set (DTRACE_SOURCES CMakeFiles/vn3d.dir/*.o)
  add_custom_command (TARGET vn3d PRE_LINK
    COMMAND rm -f dtrace.o
    COMMAND ${DTRACE_EXECUTABLE} -G -s dtrace.d ${DTRACE_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set (ADDITIONAL_LINK_FLAGS "${CMAKE_CURRENT_BINARY_DIR}/dtrace.o")
endif (DTRACE_FOUND)

set_target_properties (vn3d PROPERTIES VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld
  LINK_FLAGS "-Wl,--version-script ${CMAKE_CURRENT_BINARY_DIR}/vn3d.ld ${ADDITIONAL_LINK_FLAGS}")

install (TARGETS vn3d LIBRARY DESTINATION lib)
install (FILES vn3d.h generic.h value.h worley.h DESTINATION include/vn3d)
