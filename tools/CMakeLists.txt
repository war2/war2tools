pkg_check_modules(EET eet)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libpud)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libwar2)

if (EET_FOUND)
   add_definitions(-DHAVE_EET=1)
   include_directories(${EET_INCLUDE_DIRS})
endif ()

add_executable(ppm_cmp ppm.c ppm_cmp.c)
add_executable(tiles tiles.c ppm.c)
add_executable(tiler tiler.c)
add_executable(defaults_gen defaults_gen.c)
add_executable(tilemap tilemap.c ppm.c)

if (EET_FOUND)
   add_executable(extract_tiles extract_tiles.c)
   add_executable(extract_sprites extract_sprites.c ppm.c)
endif()

target_link_libraries(tiles ${LIBPUD_LIBRARIES})
target_link_libraries(tiler ${LIBPUD_LIBRARIES})
target_link_libraries(defaults_gen ${LIBPUD_LIBRARIES})
target_link_libraries(tilemap ${LIBPUD_LIBRARIES})
target_link_libraries(extract_tiles ${LIBWAR2_LIBRARIES})

if (EET_FOUND)
   target_link_libraries(extract_sprites ${LIBWAR2_LIBRARIES} ${EET_LIBRARIES})
   target_link_libraries(extract_tiles ${EET_LIBRARIES})
endif ()

