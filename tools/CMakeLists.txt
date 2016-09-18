pkg_check_modules(EINA eina)
pkg_check_modules(EET eet)
pkg_check_modules(ECORE_FILE ecore-file)
pkg_check_modules(EMILE emile)
pkg_check_modules(CAIRO cairo)

# FIXME Very bad cmakelists....

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libpud)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libwar2)


if (EINA_FOUND)
   include_directories(${EINA_INCLUDE_DIRS})
endif ()

if (EET_FOUND AND EMILE_FOUND)
   add_definitions(-DHAVE_EET=1)
   include_directories(${EET_INCLUDE_DIRS} ${EMILE_INCLUDE_DIRS})
endif ()

if (ECORE_FILE_FOUND)
   include_directories(${ECORE_FILE_INCLUDE_DIRS})
endif ()

if (CAIRO_FOUND)
   add_definitions(-DHAVE_CAIRO=1)
   include_directories(${CAIRO_INCLUDE_DIRS})
endif ()

add_executable(ppm_cmp ppm.c ppm_cmp.c)
add_executable(tiles tiles.c ppm.c)
add_executable(tiler tiler.c)
add_executable(defaults_gen defaults_gen.c)
add_executable(random_gen random_gen.c)
add_executable(tilemap tilemap.c ppm.c)
add_executable(opensave opensave.c)
add_executable(alow_ugrd_set alow_ugrd_set.c)

if (EET_FOUND)
   add_executable(extract_sprites extract_sprites.c ppm.c)
   add_executable(data_to_sprite data_to_sprite.c)
endif()

target_link_libraries(tiles ${LIBPUD_LIBRARIES})
target_link_libraries(tiler ${LIBPUD_LIBRARIES})
target_link_libraries(defaults_gen ${LIBPUD_LIBRARIES})
target_link_libraries(random_gen ${LIBPUD_LIBRARIES})
target_link_libraries(tilemap ${LIBPUD_LIBRARIES})
target_link_libraries(opensave ${LIBPUD_LIBRARIES})
target_link_libraries(alow_ugrd_set ${LIBPUD_LIBRARIES})

if (ECORE_FILE_FOUND)

   LIST(APPEND EXTRACT_TILES_LIBRARIES
      ${LIBWAR2_LIBRARIES}
      ${ECORE_FILE_LIBRARIES}
   )
   add_executable(extract_tiles extract_tiles.c)
endif ()


if (EINA_FOUND)
   LIST(APPEND EXTRACT_TILES_LIBRARIES
      ${EINA_LIBRARIES}
   )
endif ()
if (EET_FOUND)
   LIST(APPEND EXTRACT_TILES_LIBRARIES
      ${EET_LIBRARIES}
      ${EMILE_LIBRARIES}
   )
endif ()

if (CAIRO_FOUND)
   LIST(APPEND EXTRACT_TILES_LIBRARIES
      ${CAIRO_LIBRARIES}
   )
endif ()
target_link_libraries(extract_tiles ${EXTRACT_TILES_LIBRARIES})

if (EET_FOUND)
   target_link_libraries(extract_sprites ${LIBWAR2_LIBRARIES} ${EET_LIBRARIES} ${EMILE_LIBRARIES} ${EINA_LIBRARIES} ${CAIRO_LIBRARIES} ${ECORE_FILE_LIBRARIES})
   target_link_libraries(data_to_sprite ${EET_LIBRARIES} -lm)
endif ()

if (CAIRO_FOUND AND EET_FOUND AND ECORE_FILE_FOUND)
   add_executable(extract_icons extract_icons.c)
   target_link_libraries(extract_icons
      ${LIBPUD_LIBRARIES} ${LIBWAR2_LIBRARIES}
      ${EINA_LIBRARIES} ${EET_LIBRARIES} ${ECORE_FILE_LIBRARIES}
      ${CAIRO_LIBRARIES}
   )
endif ()
