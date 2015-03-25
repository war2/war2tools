#ifndef _GRID_H_
#define _GRID_H_

Eina_Bool bitmap_add(Editor *ed);
void bitmap_tile_set(Editor * restrict ed, int x, int y, unsigned int key);
void bitmap_sprite_draw(Editor *restrict ed, Pud_Unit unit, Pud_Player color, unsigned int orient, int x, int y);

void
bitmap_refresh_zone(Editor *restrict ed,
                    int              x,
                    int              y,
                    int              w,
                    int              h);

#endif /* ! _GRID_H_ */

