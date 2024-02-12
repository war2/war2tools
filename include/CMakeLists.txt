install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pud.h DESTINATION include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/war2.h DESTINATION include)

configure_file(pud_config.h.in pud_config.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pud_config.h DESTINATION include)
