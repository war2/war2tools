#include "war2_private.h"

static bool
_ts_entries_parse(War2_Data          *w2,
                  War2_Tileset       *ts,
                  const unsigned int *entries,
                  War2_Decode_Func    func)
{
   /* Lookup table (flip table): 0=>7, 1=>6, 2=>5, ... 7=>0
    * Thanks wargus for the tip. */
   const int ft[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };

   unsigned char *ptr, *data;
   size_t size, i;
   bool flip_x, flip_y;
   int o, x, y, j, i_img;
   Pud_Color img[1024];
   int img_ctr = 0;
   unsigned char col;

   /* Extract palette - 256x3 */
   ptr = war2_entry_extract(w2, entries[0], &size);
   if (!ptr)
     DIE_RETURN(false, "Failed to extract entry palette [%i]", entries[0]);
   if (size != 768)
     DIE_RETURN(false, "Invalid size [%zu]. Should be 256*3=768", size);
   memcpy(&(ts->palette[0]), ptr, size);
   free(ptr);
   /* I don't know why this is needed (no doc so no explaination) but this
    * gives the right colorspace (thanks wargus) */
   for (i = 0; i < size; i++)
     {
        ts->palette[i].r <<= 2;
        ts->palette[i].g <<= 2;
        ts->palette[i].b <<= 2;
     }

   /* If no callback has been specified, do nothing */
   if (!func)
     {
        WAR2_VERBOSE(w2, 1, "Warning: No callback specified.");
        return true;
     }

   /* Get minitiles info */
   ptr = war2_entry_extract(w2, entries[1], &size);
   if (!ptr)
     DIE_RETURN(false, "Failed to extract entry minitile info [%i]", entries[1]);
   data = war2_entry_extract(w2, entries[2], NULL);
   if (!data)
     {
        free(ptr);
        DIE_RETURN(false, "Failed to extract entry minitile data [%i]", entries[2]);
     }
   ts->tiles = size / 32;

   // FIXME Fog of war (16 first tiles) */

   /* Jump by blocks of 16 words */
   for (i = 0, img_ctr = 1; i < size; i += 32, img_ctr++)
     {
        /* For each word in the block of 16 */
        for (j = 0, i_img = 0; j < 32; j += 2, i_img++)
          {
             /* Get offset and flips */
             memcpy(&o, &(ptr[i + j]), sizeof(uint16_t));
             flip_x = o & 2; // 0b10
             flip_y = o & 1; // 0b01
             o = (o & 0xfffc) * 16;

             /* Decode a minitile (8x8) */
             for (y = 0; y < 8; y++)
               {
                  for (x = 0; x < 8; x++)
                    {
                       /* If flip_x/flip_y are true, the minitile must be flipped on
                        * its x/y axis. We use a flip table which avoids calculations
                        * to do so. */
                       col = data[o + ((flip_x ? ft[x] : x) + (flip_y ? ft[y] : y) * 8)];

                       /* Maths: we have 16 blocks of 8x8 to place in a 32x32
                        * image which has a linear memory layout */
                       const int xblock = x + ((i_img % 4) * 8);
                       const int yblock = y + ((i_img / 4) * 8);

                       /* Convert the byte to color thanks to the palette */
                       img[xblock + 32 * yblock] = ts->palette[col];
                    }
               }
          }

        func(img, 32, 32, ts, img_ctr);
     }

   free(ptr);
   free(data);

   return true;
}

War2_Tileset *
war2_tileset_decode(War2_Data        *w2,
                    Pud_Era           era,
                    War2_Decode_Func  func)
{
   const unsigned int forest[] = { 2, 3, 4, 5, 6, 7, 8 };
   const unsigned int wasteland[] = { 10, 11, 12, 13, 14, 15, 16 };
   const unsigned int winter[] = { 18, 19, 20, 21, 22, 23, 24 };
   const unsigned int swamp[] = { 438, 439, 440, 441, 442, 443, 444 };
   const unsigned int *entries;
   War2_Tileset *ts;

   /* Alloc */
   ts = calloc(1, sizeof(War2_Tileset));
   if (!ts) DIE_RETURN(NULL, "Failed to allocate memory");
   ts->era = era;

   switch (era)
     {
      case PUD_ERA_FOREST:    entries = forest;    break;
      case PUD_ERA_WASTELAND: entries = wasteland; break;
      case PUD_ERA_WINTER:    entries = winter;    break;
      case PUD_ERA_SWAMP:     entries = swamp;     break;
     }

   _ts_entries_parse(w2, ts, entries, func);

   return ts;
}

void
war2_tileset_free(War2_Tileset *ts)
{
   free(ts);
}

