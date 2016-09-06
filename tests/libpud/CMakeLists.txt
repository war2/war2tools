add_executable(libpud_suite
   tests.c tests.h
   test_standalone.c
)
target_include_directories(libpud_suite
   SYSTEM
   PUBLIC ${CHECK_CFLAGS}
   PUBLIC ${LIBPUD_INCLUDE_DIRS}
)
target_link_libraries(libpud_suite
   ${LIBPUD_LIBRARIES}
   ${CHECK_LDFLAGS}
)

add_test(libpud libpud_suite)
