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
   char path[1024];
   snprintf(path, sizeof(path), "sprite_%i.png", img_nb);
   war2_png_write(path, w, h, (unsigned char *)sprite);

   (void) ud;
}

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Sprites_Descriptor *ud;
   unsigned int obj;

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <unit_id>\n", argv[0]);
        return 1;
     }

   obj = strtol(argv[1], NULL, 10);
   war2_init();
   w2 = war2_open("../data/war2/maindat.war", 1);
   ud = war2_sprites_decode(w2, PUD_PLAYER_VIOLET, PUD_ERA_WINTER,
                            WAR2_SPRITES_UNITS, obj, _func);
   war2_close(w2);
   war2_shutdown();

   return 0;
}

