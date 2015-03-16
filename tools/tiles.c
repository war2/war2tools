/*
 * tiles.c
 * tiles
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "ppm.h"
#include <pud.h>

typedef struct {
   uint16_t tile;
   Col      col;
} Mem;


static inline int
_str2int(const char *str)
{
   if (!str) return -1;
   return strtol(str, NULL, 10);
}

int
main(int    argc,
     char **argv)
{
   Pud *pud;
   Col *ppm;
   int w, h, i, j;
   uint16_t word;
   int x, y;
   Mem mem[1024];
   int count = 0, k = 0, idx;
   char add;

   if ((argc < 3) || (argc == 4))
     {
        fprintf(stderr, "*** Usage: <file.pud> <file.ppm> [X, Y]\n");
        return 1;
     }

   /* Parse PUD */
   pud = pud_open(argv[1], PUD_OPEN_MODE_R);
   if (pud == NULL)
     {
        fprintf(stderr, "*** pud_new() failed\n");
        return 2;
     }
   pud_parse(pud);

   /* Parse PPM */
   ppm = ppm_parse(argv[2], &w, &h);
   if (!ppm)
     {
        fprintf(stderr, "*** ppm_parse() failed\n");
        pud_close(pud);
        return 2;
     }

   /* Get tile */
   if (argc == 5)
     {
        x = _str2int(argv[3]);
        y = _str2int(argv[4]);
     }
   else
     {
        x = -1;
        y = -1;
     }

   if ((x < 0) || (y < 0))
     {
        /* For all tiles */
        for (j = 0; j < h; j++)
          {
             for (i = 0; i < w; i++)
               {
                  word = pud_tile_at(pud, i, j);
                  if (word == 0)
                    {
                       fprintf(stderr, "*** pud_tile_at(%i, %i) failed\n", i, j);
                       pud_close(pud);
                       free(ppm);
                       return 3;
                    }
                  idx = (j * w) + i;

                  add = 1;
                  for (k = 0; k < count; k++)
                    {
                       if (mem[k].tile == word)
                         {
                            add = 0;
                            break;
                         }
                    }
                  if (add)
                    {
                       mem[count].tile = word;
                       mem[count].col = ppm[idx];
                       count++;
                    }
               }
          }
        printf("switch (tile)\n"
               "  {\n");
        for (k = 0; k < count; k++)
          {
             printf("   case 0x%04x: return _color_make(0x%02x, 0x%02x, 0x%02x);\n",
                    mem[k].tile, mem[k].col.r, mem[k].col.g, mem[k].col.b);
          }
        printf("   default: break;\n");
        printf("  }\n");
     }
   else
     {
        /* Do job for tile at X Y */
        word = pud_tile_at(pud, x, y);
        if (word == 0)
          {
             fprintf(stderr, "*** pud_tile_at() failed\n");
             pud_close(pud);
             free(ppm);
             return 3;
          }

        i = y * w + x;
        printf("0x%04x 0x%02x 0x%02x 0x%02x\n",
               word, ppm[i].r, ppm[i].g, ppm[i].b);
     }

   pud_close(pud);
   free(ppm);

   return 0;
}

