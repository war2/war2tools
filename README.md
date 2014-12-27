war2edit
========

This aims at being the clone of the Warcraft II Map Editor.

- PUD viewer
- PUD editor

pud
---

After building (assuming it is in `build/`):
- `./build/pud/pud --help`


Build
-----

war2edit uses Cmake.
If you are new to cmake:

- create a `build` directory and `cd` in: `mkdir -p build && cd build`
- run cmake: `cmake ..`
- compile all: `make`
- install: `sudo make install`
- uninstall: `sudo make uninstall`


TODO
----

- Winter color tiles (for minimap)
- Wasteland color tiles (for minimap)
- Swamp color tiles (for minimap)
- Get all tile sprites
  - summer
  - winter
  - wasteland
  - swamp
- Get all sprites
  - humans (8 colors)
  - orcs (8 colors)
  - critters
  - neutral
  - ...
- War2 map editor clone


License
-------

The MIT License (MIT)

Copyright (c) 2014 Jean Guyomarc'h

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


