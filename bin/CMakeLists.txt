pkg_check_modules(ELEMENTARY elementary)

if (ELEMENTARY_FOUND)

   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/../include
      ${ELEMENTARY_INCLUDE_DIRS}
   )

   add_definitions(-Wall)
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
      cursor.c
      mainconfig.c
      menu.c
   )
   add_executable(war2edit ${WAR2EDIT_SRC})
   add_dependencies(war2edit edje_themes)

   link_directories(${ELEMENTARY_LIBRARY_DIRS})
   target_link_libraries(war2edit ${LIBPUD_LIBRARIES} ${ELEMENTARY_LIBRARIES})

   install(
      TARGETS war2edit
      RUNTIME DESTINATION bin
   )

else()
   message(WARNING "Missing dependancies. War2Edit will not be compiled")
endif()

