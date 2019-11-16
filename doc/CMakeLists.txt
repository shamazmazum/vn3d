configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
configure_file (${CMAKE_SOURCE_DIR}/README.md ${CMAKE_CURRENT_BINARY_DIR}/README.md COPYONLY)

file (GLOB PICTURES RELATIVE ${CMAKE_SOURCE_DIR}/doc *.png)
foreach(PIC ${PICTURES})
  configure_file (${CMAKE_SOURCE_DIR}/doc/${PIC} ${CMAKE_CURRENT_BINARY_DIR}/doc/${PIC} COPYONLY)
endforeach(PIC)

add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
