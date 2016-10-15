/*
 * img.c
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

PUDAPI unsigned char *
pud_minimap_bitmap_generate(const Pud        *pud,
                            unsigned int     *size_ret,
                            Pud_Pixel_Format  pfmt)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, NULL);

   unsigned char *map;
   Pud_Unit_Data *u;
   Pud_Color c;
   unsigned int i, j, k;
   int idx;
   unsigned int size;
   uint16_t w, h;
   const Pud_Era era = pud->era;
   const unsigned int delta[2][4] = {
      [PUD_PIXEL_FORMAT_RGBA] = {
         0, 1, 2, 3
      },
      [PUD_PIXEL_FORMAT_ARGB] = {
         2, 1, 0, 3
      }
   };

   size = pud->tiles * 4;
   map = malloc(size * sizeof(unsigned char));
   if (!map) DIE_RETURN(NULL, "Failed to allocate memory");

   for (i = 0, idx = 0; i < pud->tiles; i++, idx += 4)
     {
        c = pud_tile_to_color(era, pud->tiles_map[i]);

        map[idx + delta[pfmt][0]] = c.r;
        map[idx + delta[pfmt][1]] = c.g;
        map[idx + delta[pfmt][2]] = c.b;
        map[idx + delta[pfmt][3]] = c.a;
     }

   for (i = 0; i < pud->units_count; i++)
     {
        u = &(pud->units[i]);
        c = pud_color_for_unit(u->type, u->owner);

        w = pud->unit_data[u->type].size_w;
        h = pud->unit_data[u->type].size_h;

        for (j = 0; j < w; j++)
          {
             for (k = 0; k < h; k++)
               {
                  idx = (((u->y + k)* pud->map_w) + (u->x + j)) * 4;

                  map[idx + delta[pfmt][0]] = c.r;
                  map[idx + delta[pfmt][1]] = c.g;
                  map[idx + delta[pfmt][2]] = c.b;
                  map[idx + delta[pfmt][3]] = c.a;
               }
          }
     }

   if (size_ret) *size_ret = size;

   return map;
}
