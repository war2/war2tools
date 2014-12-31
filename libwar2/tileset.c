#include "war2_private.h"

static bool
_ts_entries_parse(War2_Data          *w2,
                  War2_Tileset       *ts,
                  const unsigned int *entries)
{
   /* Lookup table (flip table): 0=>7, 1=>6, 2=>5, ... 7=>0 */
   const int ft[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
   unsigned char *ptr, *p;
   unsigned char *data, *pdata;
   size_t size, tiles, dsize, i;
   int o;
   bool flip_x, flip_y;
   int x, y, px, py;
   int height, width;

   /* Extract palette - 256x3 */
   ptr = war2_entry_extract(w2, entries[0], &size);
   if (!ptr)
     DIE_RETURN(false, "Failed to extract entry palette [%i]", entries[0]);
   if (size != 768)
     DIE_RETURN(false, "Invalid size [%zu]. Should be 256*3=768", size);
   memcpy(&(ts->palette[0]), ptr, size);
   free(ptr);
   for (i = 0; i < size; i++)
     {
        ts->palette[i].r <<= 2;
        ts->palette[i].g <<= 2;
        ts->palette[i].b <<= 2;
     }

   /* Get minitiles info */
   ptr = war2_entry_extract(w2, entries[1], &size);
   if (!ptr)
     DIE_RETURN(false, "Failed to extract entry minitile info [%i]", entries[1]);
   data = war2_entry_extract(w2, entries[2], &dsize);
   if (!data)
     {
        free(ptr);
        DIE_RETURN(false, "Failed to extract entry minitile data [%i]", entries[2]);
     }
   pdata = data;


   int cptr = 0;
   int j;
   Pud_Color img[1024];
   int i_img;
   memset(&(img[0]), 0, sizeof(img));
   for (i = 0; i < size; i += 32) /* Jump by blocks of 16 words */
     {
        char bb[128];
        snprintf(bb, 128, "img_%i.ppm", ++cptr);
        FILE *f = fopen(bb, "w+");
        fprintf(f, "P3\n32 32\n255\n");

        /* For each word in the block of 16 */
        for (j = 0, i_img = 0; j < 32; j += 2, i_img++)
          {
             memcpy(&o, &(ptr[i + j]), sizeof(uint16_t));
             flip_x = o & 2; // 0b10
             flip_y = o & 1; // 0b01
             o = (o & 0xfffc) * 16;

             for (y = 0; y < 8; y++)
               {
                  for (x = 0; x < 8; x++)
                    {
                       unsigned char col = data[o + ((flip_x ? ft[x] : x) + (flip_y ? ft[y] : y) * 8)];
                       const int xblock = x + ((i_img % 4) * 8);
                       const int yblock = y + ((i_img / 4) * 8);
                       img[xblock + 32 * yblock] = ts->palette[col];
                     //  fprintf(f, "%i %i %i\n",
                     //          ts->palette[col].r,
                     //          ts->palette[col].g,
                     //          ts->palette[col].b);
                    }
               }
          }
        int z;
        for (z = 0; z < 1024; z++)
          fprintf(f, "%i %i %i\n", img[z].r, img[z].g, img[z].b);
        fclose(f);
     }

#if 0
#define TPS 16 /* Tiles Per Row. S = ?? Wtf did I just do? */
   tiles = size / 32;
   width = TPS * 32;
   height = ((tiles + TPS - 1) / TPS) * 32;
   ts->img = calloc(width * height, sizeof(unsigned char));

   for (i = 16; i < tiles; i++) /* Skip the fog of war */
     {
        p = ptr + i * 32;
        for (y = 0; y < 4; y++)
          {
             for (x = 0; x < 4; x++)
               {
                  /* Get tile flips and offset */
                  memcpy(&o, &(p[x + (y * 4)]), sizeof(uint16_t));
                  flip_x = o & 2; /* 0b0010 */
                  flip_y = o & 1; /* 0b0001 */
                  o = (o & 0xfffc) * 16;

                  for (py = 0; py < 8; py++)
                    {
                       for (px = 0; px < 8; px++)
                         {
                            int ix = x + ((i % TPS) * 4);
                            int iy = y + ((i / TPS) * 4);
                            ts->img[(py + iy * 8) * width + px + (ix * 8)] =
                               pdata[o + px + py * 8];
                               //pdata[o + (flip_y ? flip[py] : py) * 8 + (flip_x ? flip[px] : px)];
                         }
                    }
               }
          }
     }
   FILE *f = fopen("img.ppm", "w+");
   fprintf(f, "P3\n%i %i\n255\n", width, height);
   for (y = 0; y < height; y++)
     {
        for (x = 0; x < width; x++)
          {
             int iii = (y * width) + x;
           //  printf("(%i,%i) %i %i\n", width, height, width * height, iii);

             unsigned char idx = ts->img[iii];
             Pud_Color c = ts->palette[idx];
             fprintf(f, "%i %i %i\n", c.r, c.g, c.b);
          }
     }

   fclose(f);
   printf("Size: %zu. Tiles: %zu\n", size, size /32);
#endif

   free(ptr);
   free(data);

   return true;
}



War2_Tileset *
war2_tileset_decode(War2_Data *w2,
                    Pud_Era    era)
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

   _ts_entries_parse(w2, ts, entries);
  // READBUF(w2, &(ts->palette[0]), Pud_Color, 256, ECHAP(err_free));

   return ts;

err_free:
   free(ts);
   return NULL;
}

void
war2_tileset_free(War2_Tileset *ts)
{
   free(ts);
}

