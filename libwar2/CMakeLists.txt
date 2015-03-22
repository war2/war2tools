set(BUILD_SHARED_LIBS true)

add_definitions(-Wall)
add_definitions(-std=c99)

add_library(libwar2 SHARED
   war2.c
   private.c
   tileset.c
   units.c
)
SET_TARGET_PROPERTIES(libwar2 PROPERTIES PREFIX "")

target_include_directories(
   libwar2
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(${LIBWAR2_LIBRARIES})

install(
   TARGETS libwar2
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib/static
)

