/*
 * img.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

unsigned char *
pud_minimap_bitmap_generate(Pud *pud,
                            int *size_ret)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, NULL);

   unsigned char *map;
   struct _unit *u;
   Pud_Color c;
   unsigned int i, j, k;
   int idx;
   int size;
   uint16_t w, h;

   size = pud->tiles * 4;
   map = malloc(size * sizeof(unsigned char));
   if (!map) DIE_RETURN(NULL, "Failed to allocate memory");

   for (i = 0, idx = 0; i < pud->tiles; i++, idx += 4)
     {
        c = pud_tile_to_color(pud, pud->tiles_map[i]);

        map[idx + 0] = c.r;
        map[idx + 1] = c.g;
        map[idx + 2] = c.b;
        map[idx + 3] = c.a;
     }

   for (i = 0; i < pud->units_count; i++)
     {
        u = &(pud->units[i]);
        if (u->type == PUD_UNIT_GOLD_MINE)
          c = color_make(0xff, 0xff, 0x00, 0xff);
        else if (u->type == PUD_UNIT_OIL_PATCH)
          c = color_make(0x00, 0x00, 0x00, 0xff);
        else
          c = color_for_player(u->owner);

        w = pud->unit_data[u->type].size_w;
        h = pud->unit_data[u->type].size_h;

        for (j = 0; j < w; j++)
          {
             for (k = 0; k < h; k++)
               {
                  idx = (((u->y + k)* pud->map_w) + (u->x + j)) * 4;

                  map[idx + 0] = c.r;
                  map[idx + 1] = c.g;
                  map[idx + 2] = c.b;
                  map[idx + 3] = c.a;
               }
          }
     }

   if (size_ret) *size_ret = size;

   return map;
}

