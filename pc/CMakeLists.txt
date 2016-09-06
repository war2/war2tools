configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pud.pc.in ${CMAKE_CURRENT_BINARY_DIR}/pud.pc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/war2.pc.in ${CMAKE_CURRENT_BINARY_DIR}/war2.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pud.pc DESTINATION lib/pkgconfig)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/war2.pc DESTINATION lib/pkgconfig)
