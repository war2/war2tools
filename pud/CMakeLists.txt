include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../include
   ${PUD_INCLUDES_DIR}
)

set(PUD_SRC main.c ppm.c jpeg.c png.c)
add_executable(pud ${PUD_SRC})

target_link_libraries(pud ${PUD_LIBRARIES} ${LIBWAR2_LIBRARIES})

install(
   TARGETS pud
   RUNTIME DESTINATION bin
)

