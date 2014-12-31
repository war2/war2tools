cmake_minimum_required(VERSION 2.8)
project(war2edit C)

add_definitions(-g)
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

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(LIBPUD_LIBRARIES  libpud ${JPEG_LIBRARIES} ${PNG_LIBRARIES})
set(LIBWAR2_LIBRARIES libwar2 libpud)

add_subdirectory(libpud)
add_subdirectory(libwar2)
add_subdirectory(pc)
add_subdirectory(pud)
add_subdirectory(include)
add_subdirectory(tools)
add_subdirectory(bin)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake)

