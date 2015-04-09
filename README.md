war2edit
========

This repository contains tools to manipulate PUD files. PUDs are Warcraft II map files.
The build system used is `cmake`:

- `mkdir -p build && cd build`
- `make`

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


TODO:
- [ ] `pud_mmap()` is not implemented on systems which do not provide `mmap()`
- [ ] `pud_munmap()` is not implemented on systems which do not provide `munmap()`


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
- [ ] Icons' sprites
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

war2edit
--------

A clone of Warcraft II World Map Editor.
It relies on the EFL, Elementary, Elm_Bitmap (https://github.com/jeanguyomarch/elm_bitmap), libpud and libwar2.

TODO:
- [ ] Lock cursor
- [ ] Properties (players, units, upgrades, start, map)
- [ ] Load a file
- [ ] Tiles !!!!
- [ ] About


tools
-----

Those binaries are mostly helpers to create the libraries.
- `defaults_gen`: used to generate parts of `libpud/defaults.c`
- `extract_tiles`: used to extract tiles from maindat.war (in `data/tiles`)
- `extract_sprites`: used to extract units and buildings' sprites from maindat.war (in `data/sprites`)
- `ppm_cmp`: makes a `diff` between 2 ppm files
- `tilemap`: used to generate parts of `libpud/tiles.c`
- `tiler`: generates a map with all possible tiles
- `tiles`: used to generate parts of `libpud/tiles.c`


License
=======

All resources (sprites, tiles, .PUD, .war) are property of Blizzard Entertainment.
The following license applies only to all source files.


The MIT License (MIT)

Copyright (c) 2014 - 2015 Jean Guyomarc'h

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


