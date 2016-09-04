/*
 * ppm.c
 * pud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pud_private.h"

Pud_Bool
pud_minimap_to_ppm(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, PUD_FALSE);

   FILE *f;
   unsigned int i, size;
   unsigned char *map;

   map = pud_minimap_bitmap_generate(pud, &size);
   if (!map) DIE_RETURN(PUD_FALSE, "Failed to generate bitmap");

   f = fopen(file, "w");
   if (!f) DIE_RETURN(PUD_FALSE, "Failed to open [%s]", file);

   /* Write PPM header */
   fprintf(f,
           "P3\n"
           "%i %i\n"
           "255\n",
           pud->map_w, pud->map_h);

   for (i = 0; i < size; i += 4)
     {
        fprintf(f, "%i %i %i\n",
                map[i + 0],
                map[i + 1],
                map[i + 2]);
     }
   fclose(f);
   free(map);

   PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return PUD_TRUE;
}
