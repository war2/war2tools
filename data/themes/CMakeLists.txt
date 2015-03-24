find_program(EDJE_CC edje_cc)

if (EDJE_CC)

   # Generic hook
   add_custom_target(edje_themes)

   set(EDC ${CMAKE_CURRENT_SOURCE_DIR}/default.edc)
   set(EDJ ${CMAKE_CURRENT_BINARY_DIR}/default.edj)
   add_custom_command(
      OUTPUT ${EDJ}
      COMMAND ${EDJE_CC} -id ${CMAKE_CURRENT_SOURCE_DIR}/images ${EDC} ${EDJ}
      VERBATIM
      COMMENT "Generating ${EDJ}"
      DEPENDS ${EDC}
   )
   add_custom_target(edje_theme_default DEPENDS ${EDJ})
   add_dependencies(edje_themes edje_theme_default)

endif()

