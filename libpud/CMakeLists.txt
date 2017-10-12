set(libpud_src 
   pud.c
   common.c
   defaults.c
   img.c
   parse.c
   private.c
   tiles.c
   utils.c
   random.c
)

if (MSVC)
	add_library(libpud STATIC ${libpud_src})
else ()
	add_library(libpud SHARED ${libpud_src})
endif ()

target_include_directories(libpud
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Disable the prefix because it is already in the name
# The name cannot (?) be changed because there is another target called pud
SET_TARGET_PROPERTIES(libpud PROPERTIES PREFIX "")

install(
   TARGETS libpud
   RUNTIME DESTINATION bin COMPONENT runtime
   ARCHIVE DESTINATION lib COMPONENT devel
   LIBRARY DESTINATION lib COMPONENT devel
)
