set(BUILD_SHARED_LIBS true)

set(
   LIBPUD_SRC
   pud.c
   defaults.c
   img.c
   parse.c
   print.c
   private.c
   tiles.c
   utils.c
   mmap.c
)

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../include
)


add_definitions(-Wall)
add_definitions(-std=c99)

add_library(libpud SHARED ${LIBPUD_SRC})
add_library(libpud_static STATIC ${LIBPUD_SRC})

# Disable the prefix because it is already in the name
# The name cannot (?) be changed because there is another target called pud
SET_TARGET_PROPERTIES(libpud PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(libpud_static PROPERTIES OUTPUT_NAME libpud PREFIX "")

install(
   TARGETS libpud
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib/static
)

