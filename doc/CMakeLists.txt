configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_pud.in
   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_pud
   @ONLY
)

add_custom_target(doc
   COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_pud
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_pud
)
