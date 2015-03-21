#ifndef _GRID_H_
#define _GRID_H_


struct _Grid_Cell
{
   unsigned int tile : 9;
   unsigned int unit : 7;
   unsigned int orient : 4;
   unsigned int player : 3;
   unsigned int anchor : 1;
};


Eina_Bool bitmap_add(Editor *ed);
void bitmap_tile_set(Editor * restrict ed, int x, int y, unsigned int key);

#endif /* ! _GRID_H_ */

