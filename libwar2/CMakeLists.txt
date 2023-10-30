if (JPEG_FOUND)
   add_definitions(-DHAVE_JPEG=1)
endif()
if (PNG_FOUND)
   add_definitions(-DHAVE_PNG=1)
endif()

set(libwar2_src
   war2.c
   tileset.c
   font.c
   ui.c
   sprites.c
   cursors.c
   png.c
   jpeg.c
   ppm.c
)

if (MSVC)
	add_library(libwar2 STATIC ${libwar2_src})
else ()
	add_library(libwar2 SHARED ${libwar2_src})
endif ()

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
   RUNTIME DESTINATION bin COMPONENT runtime
   ARCHIVE DESTINATION lib COMPONENT devel
   LIBRARY DESTINATION lib COMPONENT devel
)
