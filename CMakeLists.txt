cmake_minimum_required(VERSION 3.0)
project(war2tools C)

add_definitions(-g)
add_definitions(-Wall)
add_definitions(-W)
add_definitions(-Wshadow)
add_definitions(-std=gnu99)
add_definitions(-D_BSD_SOURCE)
add_definitions(-D_DEFAULT_SOURCE)

set(PROJECT_DESCRIPTION "Warcraft II Map Editor Toolkit")
set(PROJECT_URL "")

set(LIBPUD_DESCRIPTION "Warcraft II PUD Manipulation Utilities")
set(LIBPUD_VERSION_MAJOR 0)
set(LIBPUD_VERSION_MINOR 99)

set(LIBWAR2_DESCRIPTION "Warcraft II Data Files Manipulation Utilities")
set(LIBWAR2_VERSION_MAJOR 0)
set(LIBWAR2_VERSION_MINOR 99)

set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

find_package(PkgConfig)
find_package(JPEG)
find_package(PNG)

include(cmake/copy_file.cmake)

set(LIBWAR2_LIBRARIES libwar2 libpud)
set(LIBWAR2_INCLUDE_DIRS "")
set(LIBPUD_LIBRARIES libpud)

if (JPEG_FOUND)
   set(LIBWAR2_INCLUDE_DIRS ${LIBWAR2_INCLUDES_DIRS} ${JPEG_INCLUDES_DIR})
   set(LIBWAR2_LIBRARIES ${LIBWAR2_LIBRARIES} ${JPEG_LIBRARIES})
endif ()
if (PNG_FOUND)
   set(LIBWAR2_INCLUDES_DIRS ${LIBWAR2_INCLUDES_DIRS} ${PNG_INCLUDES_DIR})
   set(LIBWAR2_LIBRARIES ${LIBWAR2_LIBRARIES} ${PNG_LIBRARIES})
endif ()

set(PUD_LIBRARIES libpud ${LIBWAR2_LIBRARIES})
set(PUD_INCLUDES_DIRS ${LIBWAR2_INCLUDE_DIRS})

add_subdirectory(libpud)
add_subdirectory(libwar2)
add_subdirectory(pc)
add_subdirectory(pud)
add_subdirectory(include)
add_subdirectory(tools)
add_subdirectory(quicklook)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake)
