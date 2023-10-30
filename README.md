war2tools
=========

<a href="https://scan.coverity.com/projects/jeanguyomarch-war2tools">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/6937/badge.svg"/>
</a>

This repository contains tools to manipulate PUD files. PUDs are Warcraft II map files.
The build system used is `cmake`:

- `mkdir -p build && cd build`
- `cmake ..`, or `cmake -DBUILD_TOOLS=ON ..` if you want to build development tools
  bundled with war2tools. You are probably not interested in them, and they will
  not be installed.
- `cmake --build .`

To install (as root):
- `cmake --build . --target install`
- Possibly `ldconfig` on GNU/Linux (as root)


libpud
------

A C library to manipulate a PUD file:
- extract all data from it;
- generate a custom PUD file;
- generate a bitmap of the minimap.


libwar2
-------

A C library which aims at extracting data from Warcraft II data files to get:
- tilesets;
- units and buildings' sprites;
- musics and sounds;
- cinematics.


Graphical modules (optionals):
- libpng
- jpeglib


TODO:
- [ ] Musics
- [ ] Sounds
- [ ] Cinematics


pud
---

A small tool that allows to interact with `libpud`.
It allows to export the minimap of a PUD to an image. Supported formats:
- jpg
- png
- ppm

To disable the compilation of `pud`, pass `-DBUILD_PUD_UTIL=OFF` to cmake.

tools
-----

Those binaries are mostly helpers to create the libraries.
- `defaults_gen`: used to generate parts of `libpud/defaults.c`
- `extract_tiles`: used to extract tiles from maindat.war
- `extract_sprites`: used to extract units and buildings' sprites from maindat.war
- `extract_icons`: used to extract icons' sprites from maindat.war
- `ppm_cmp`: makes a `diff` between 2 ppm files
- `tilemap`: used to generate parts of `libpud/tiles.c`
- `tiler`: generates a map with all possible tiles
- `tiles`: used to generate parts of `libpud/tiles.c`

To enable the compilation of the tools, pass `-DBUILD_TOOLS=ON` to cmake.`


License
=======

All resources (sprites, tiles, .PUD, .WAR) are property of Blizzard Entertainment.
Documentation is taken from multiple internet sources, which are referenced when used.

war2tools source code (libpud, libwar2, include files, tools) are under the MIT license.
For more details, please refer to the `LICENSE` file.
