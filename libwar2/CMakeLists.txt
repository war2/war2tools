set(BUILD_SHARED_LIBS true)

if (JPEG_FOUND)
   add_definitions(-DHAVE_JPEG=1)
endif()
if (PNG_FOUND)
   add_definitions(-DHAVE_PNG=1)
endif()

add_library(libwar2 SHARED
   war2.c
   private.c
   tileset.c
   sprites.c
   png.c
   jpeg.c
   ppm.c
)
SET_TARGET_PROPERTIES(libwar2 PROPERTIES PREFIX "")

target_include_directories(
   libwar2
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
   PUBLIC ${LIBWAR2_INCLUDE_DIRS}
)

target_link_libraries(${LIBWAR2_LIBRARIES})

install(
   TARGETS libwar2
   LIBRARY DESTINATION lib
)
