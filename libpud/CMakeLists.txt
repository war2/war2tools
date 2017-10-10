add_library(libpud
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
