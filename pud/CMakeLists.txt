add_executable(pud
   main.c
   ppm.c
   jpeg.c
   png.c
)

target_include_directories(
   pud
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
   PUBLIC ${PUD_INCLUDES_DIR}
)

target_link_libraries(pud ${PUD_LIBRARIES})

install(
   TARGETS pud
   RUNTIME DESTINATION bin
)

install(
   PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/pud2png
   DESTINATION bin
)
