add_executable(libpud_suite
   tests.c tests.h
   test_standalone.c
   test_open.c
)
target_include_directories(libpud_suite
   SYSTEM
   PUBLIC ${CMAKE_SOURCE_DIR}/include
   PUBLIC ${CHECK_CFLAGS}
)
target_link_libraries(libpud_suite
   ${LIBPUD_LIBRARIES}
   ${CHECK_LDFLAGS}
)

add_test(libpud libpud_suite)
