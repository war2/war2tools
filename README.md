war2tools
=========

<a href="https://scan.coverity.com/projects/jeanguyomarch-war2tools">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/6937/badge.svg"/>
</a>

This repository contains tools to manipulate PUD files. PUDs are Warcraft II map files.
The build system used is `cmake`:

- `mkdir -p build && cd build`
- `cmake --build`

To install:
- `sudo make install`
- Possibly `sudo ldconfig` on GNU/Linux

To uninstall:
- `sudo make uninstall`


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


quicklook
---------

Quicklook plugin for OSX. Allows to preview minimaps of PUD files in the Finder.
Installation steps:
- `make quicklook`
- `sudo cp -r build/quicklook/Pud.qlgenerator /Library/QuickLook`


pud
---

A small tool that allows to interact with `libpud`.
It allows to export the minimap of a PUD to an image. Supported formats:
- jpg
- png
- ppm

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


License
=======

All resources (sprites, tiles, .PUD, .WAR) are property of Blizzard Entertainment.
Documentation is taken from multiple internet sources, which are referenced when used.

war2tools source code (libpud, libwar2, include files, tools) are under the MIT license.
For more details, please refer to the `LICENSE` file.
