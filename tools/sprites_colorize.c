/*
 * sprites_colorize.c
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "ppm.h"
#include "pud.h"
#include "war2.h"


static void
_func(const Pud_Color               *sprite,
      int                            w,
      int                            h,
      const War2_Sprites_Descriptor *ud,
      int                            img_nb)
{
   const int size = w * h;
   char path[1024];
   int i;
   Col *ppm;
   snprintf(path, sizeof(path), "sprite_%i.ppm", img_nb);

   ppm = malloc(sizeof(Col) * size);

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
   War2_Sprites_Descriptor *ud;

   war2_init();
   w2 = war2_open("../data/war2/maindat.war", 1);
   ud = war2_sprites_decode(w2, PUD_PLAYER_VIOLET, PUD_SIDE_ORC, _func);
   war2_close(w2);
   war2_shutdown();

   return 0;
}

