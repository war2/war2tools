/*
 * sprites_colorize.c
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "ppm.h"
#include "pud.h"
#include "war2.h"

static const Col _colors[8][4] =
{
   { /* Red */
        { 0x44, 0x04, 0x00 },
        { 0x5c, 0x04, 0x00 },
        { 0x7c, 0x00, 0x00 },
        { 0xa4, 0x00, 0x00 },
   },
   { /* Blue */
        { 0x00, 0x04, 0x4c },
        { 0x00, 0x14, 0x6c },
        { 0x00, 0x24, 0x94 },
        { 0x00, 0x3c, 0xc0 }
   },
   { /* Green */
        { 0x00, 0x3c, 0xc0 },
        { 0x04, 0x54, 0x2c },
        { 0x14, 0x84, 0x5c },
        { 0x2c, 0xb4, 0x94 }
   },
   { /* Violet */
        { 0x2c, 0x08, 0x2c },
        { 0x50, 0x10, 0x4c },
        { 0x74, 0x30, 0x84 },
        { 0x98, 0x48, 0xb0 }
   },
   { /* Orange */
        { 0x6e, 0x20, 0x0c },
        { 0x98, 0x38, 0x10 },
        { 0xc4, 0x58, 0x10 },
        { 0xf0, 0x84, 0x14 }
   },
   { /* Black */
        { 0x0c, 0x0c, 0x14 },
        { 0x14, 0x14, 0x20 },
        { 0x1c, 0x1c, 0x2c },
        { 0x28, 0x28, 0x3c }
   },
   { /* White */
        { 0x24, 0x28, 0x4c },
        { 0x54, 0x54, 0x80 },
        { 0x98, 0x98, 0xb4 },
        { 0xe0, 0xe0, 0xe0 }
   },
   { /* Yellow */
        { 0xb4, 0x74, 0x00 },
        { 0xcc, 0xa0, 0x10 },
        { 0xe4, 0xcc, 0x28 },
        { 0xfc, 0xfc, 0x48 }
   }
};

static void
_col_convert_to(Col        *col,
                Pud_Player  color)
{
   unsigned int i;

   for (i = 0; i < 4; ++i)
     {
        if (!memcmp(col, &(_colors[0][i]), sizeof(Col)))
          {
             memcpy(col, &(_colors[color][i]), sizeof(Col));
             break;
          }
     }
}

static void
_func(const Pud_Color             *sprite,
      int                          w,
      int                          h,
      const War2_Units_Descriptor *ud,
      int                          img_nb)
{
   const int size = w * h;
   char path[1024];
   int i;
   Col *ppm;
   snprintf(path, sizeof(path), "sprite_%i.ppm", img_nb);

   ppm = malloc(sizeof(Col) * w * h);

   for (i = 0; i < size; ++i)
     {
        ppm[i].r = sprite[i].r;
        ppm[i].g = sprite[i].g;
        ppm[i].b = sprite[i].b;
        /* Ignore alpha */
     }

   ppm_save(path, ppm, w, h);
   free(ppm);
}

int
main(void)
{
   War2_Data *w2;
   War2_Units_Descriptor *ud;
   war2_init();

   w2 = war2_open("../data/war2/maindat.war", 3);

   ud = war2_units_decode(w2, PUD_PLAYER_VIOLET, PUD_SIDE_ORC, _func);
   war2_close(w2);

   war2_shutdown();
   /*
   Col *ptr;
   int w, h, i, size;

   ptr = ppm_parse("balista.ppm", &w, &h);
   if (!ptr) return -1;

   size = w * h;
   for (i = 0; i < size; i++)
     _col_convert_to(&(ptr[i]), PUD_PLAYER_BLUE);
   ppm_save("balista_blue.ppm", ptr, w, h);
   */

   return 0;
}

