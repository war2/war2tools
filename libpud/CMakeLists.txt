set(
   LIBPUD_SRC
   pud.c
   defaults.c
   img.c
   parse.c
   private.c
   tiles.c
   utils.c
   random.c
)

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_library(libpud ${LIBPUD_SRC})

# Disable the prefix because it is already in the name
# The name cannot (?) be changed because there is another target called pud
SET_TARGET_PROPERTIES(libpud PROPERTIES PREFIX "")

install(
   TARGETS libpud
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
