#include "pud_private.h"

unsigned char *
pud_minimap_bitmap_generate(Pud *pud,
                            int *size_ret)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, NULL);

   unsigned char *map;
   struct _unit *u;
   Pud_Color c;
   int i, j, k;
   int idx;
   int size;
   uint16_t w, h;

   size = pud->tiles * 3;
   map = calloc(size, sizeof(unsigned char));
   if (!map) DIE_RETURN(NULL, "Failed to allocate memory");

   for (i = 0, idx = 0; i < pud->tiles; i++, idx += 3)
     {
        c = pud_tile_to_color(pud, pud->tiles_map[i]);

        map[idx + 0] = c.r;
        map[idx + 1] = c.g;
        map[idx + 2] = c.b;
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
                  idx = (((u->y + k)* pud->map_w) + (u->x + j)) * 3;

                  map[idx + 0] = c.r;
                  map[idx + 1] = c.g;
                  map[idx + 2] = c.b;
               }
          }
     }

   if (size_ret) *size_ret = size;

   return map;
}

bool
pud_minimap_to_ppm(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   FILE *f;
   int i, size;
   unsigned char *map;

   map = pud_minimap_bitmap_generate(pud, &size);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   f = fopen(file, "w");
   if (!f) DIE_RETURN(false, "Failed to open [%s]", file);

   /* Write PPM header */
   fprintf(f,
           "P3\n"
           "%i %i\n"
           "255\n",
           pud->map_w, pud->map_h);

   for (i = 0; i < size; i += 3)
     {
        fprintf(f, "%i %i %i\n",
                map[i + 0],
                map[i + 1],
                map[i + 2]);
     }
   fclose(f);
   free(map);

   PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return true;
}

bool
pud_minimap_to_jpeg(Pud        *pud,
                    const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   unsigned char *map;
   bool chk;

   map = pud_minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = pud_jpeg_write(file, pud->map_w, pud->map_h, map);
   free(map);

   if (chk)
     PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return chk;
}

bool
pud_minimap_to_png(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   unsigned char *map;
   bool chk;

   map = pud_minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = pud_png_write(file, pud->map_w, pud->map_h, map);
   free(map);

   if (chk)
     PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return chk;
}

