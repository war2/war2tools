pkg_check_modules(EET eet)
pkg_check_modules(ECORE_FILE ecore-file)
pkg_check_modules(EMILE emile)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libpud)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libwar2)

if (EET_FOUND AND EMILE_FOUND)
   add_definitions(-DHAVE_EET=1)
   include_directories(${EET_INCLUDE_DIRS} ${EMILE_INCLUDE_DIRS})
endif ()

if (ECORE_FILE_FOUND)
   include_directories(${ECORE_FILE_INCLUDE_DIRS})
endif ()

add_executable(ppm_cmp ppm.c ppm_cmp.c)
add_executable(tiles tiles.c ppm.c)
add_executable(tiler tiler.c)
add_executable(defaults_gen defaults_gen.c)
add_executable(random_gen random_gen.c)
add_executable(tilemap tilemap.c ppm.c)

if (EET_FOUND)
   add_executable(extract_sprites extract_sprites.c ppm.c)
   add_executable(data_to_sprite data_to_sprite.c)
endif()

target_link_libraries(tiles ${LIBPUD_LIBRARIES})
target_link_libraries(tiler ${LIBPUD_LIBRARIES})
target_link_libraries(defaults_gen ${LIBPUD_LIBRARIES})
target_link_libraries(random_gen ${LIBPUD_LIBRARIES})
target_link_libraries(tilemap ${LIBPUD_LIBRARIES})

if (EET_FOUND)
   target_link_libraries(extract_sprites ${LIBWAR2_LIBRARIES} ${EET_LIBRARIES} ${EMILE_LIBRARIES})
   target_link_libraries(data_to_sprite ${EET_LIBRARIES} -lm)

   if (ECORE_FILE_FOUND)
      add_executable(extract_tiles extract_tiles.c)
      target_link_libraries(
         extract_tiles
         ${LIBWAR2_LIBRARIES}
         ${ECORE_FILE_LIBRARIES}
         ${EET_LIBRARIES}
         ${EMILE_LIBRARIES}
      )
   endif ()
endif ()

