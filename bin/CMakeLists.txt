pkg_check_modules(ELEMENTARY elementary)
pkg_check_modules(ELM_BITMAP elm-bitmap)

if (ELEMENTARY_FOUND AND ELM_BITMAP_FOUND)

   add_definitions(-DPACKAGE_COMPILE_BIN_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\")
   add_definitions(-DPACKAGE_COMPILE_DATA_DIR=\"${CMAKE_BINARY_DIR}/data\")

   set(
      WAR2EDIT_SRC
      main.c
      editor.c
      log.c
      bitmap.c
      texture.c
      sprite.c
      file.c
      mainconfig.c
      menu.c
      toolbar.c
      cell.c
   )
   add_executable(war2edit ${WAR2EDIT_SRC})

   target_include_directories(
      war2edit
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
      PUBLIC ${ELEMENTARY_INCLUDE_DIRS}
      PUBLIC ${ELM_BITMAP_INCLUDE_DIRS}
   )

   target_link_libraries(war2edit
      ${LIBPUD_LIBRARIES}
      ${LIBWAR2_LIBRARIES}
      ${ELEMENTARY_LIBRARIES}
      ${ELM_BITMAP_LIBRARIES}
   )

   install(
      TARGETS war2edit
      RUNTIME DESTINATION bin
   )

else()
   message(WARNING "Missing dependancies. War2Edit will not be compiled")
endif()

