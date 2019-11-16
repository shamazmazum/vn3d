include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../src ${TURBOJPEG_INCLUDE_DIR})
add_executable (vn3dgen vn3dgen.c)
target_link_libraries (vn3dgen vn3d ${TURBOJPEG_LIBRARY})
install (TARGETS vn3dgen RUNTIME DESTINATION bin)
