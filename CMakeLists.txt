cmake_minimum_required(VERSION 2.8)
project(war2edit C)

add_definitions(-g)
add_definitions(-Wall)
add_definitions(-W)
add_definitions(-Wshadow)
add_definitions(-D_BSD_SOURCE)

set(PROJECT_DESCRIPTION "Warcraft II Map Editor Toolkit")
set(PROJECT_URL "")

set(LIBPUD_DESCRIPTION "Warcraft II PUD Manipulation Utilities")
set(LIBPUD_VERSION_MAJOR 1)
set(LIBPUD_VERSION_MINOR 0)

set(LIBWAR2_DESCRIPTION "Warcraft II Data Files Manipulation Utilities")
set(LIBWAR2_VERSION_MAJOR 1)
set(LIBWAR2_VERSION_MINOR 0)

set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MACOSX_RPATH OFF)

find_package(PkgConfig)
find_package(JPEG)
find_package(PNG)

set(PUD_LIBRARIES libpud)
set(PUD_INCLUDES_DIRS "")

if (JPEG_FOUND)
   set(PUD_INCLUDES_DIRS ${PUD_INCLUDES_DIRS} ${JPEG_INCLUDES_DIR})
   set(PUD_LIBRARIES ${PUD_LIBRARIES} ${JPEG_LIBRARIES})
endif ()
if (PNG_FOUND)
   set(PUD_INCLUDES_DIRS ${PUD_INCLUDES_DIRS} ${PNG_INCLUDES_DIR})
   set(PUD_LIBRARIES ${PUD_LIBRARIES} ${PNG_LIBRARIES})
endif ()

set(LIBPUD_LIBRARIES libpud)
set(LIBWAR2_LIBRARIES libwar2 libpud)

add_subdirectory(libpud)
add_subdirectory(libwar2)
add_subdirectory(pc)
add_subdirectory(pud)
add_subdirectory(include)
add_subdirectory(tools)
add_subdirectory(bin)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake)

