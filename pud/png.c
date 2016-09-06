/*
 * png.c
 * pud
 *
 * Copyright (c) 2014 - 2015 Jean Guyomarc'h
 */

#include "war2.h"
#include "pud_private.h"


Pud_Bool
pud_minimap_to_png(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   unsigned char *map;
   Pud_Bool chk;

   map = pud_minimap_bitmap_generate(pud, NULL, PUD_PIXEL_FORMAT_RGBA);
   if (!map) DIE_RETURN(PUD_FALSE, "Failed to generate bitmap");

   chk = war2_png_write(file, pud->map_w, pud->map_h, map);
   free(map);

   if (chk)
     PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return chk;
}
