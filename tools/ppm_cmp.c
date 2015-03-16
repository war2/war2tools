/*
 * ppm_cmp.c
 * ppm_cmp
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "ppm.h"

static void
_usage(FILE *stream)
{
   fprintf(stream,
           "Usage: ppm_cmp <file1.ppm> <file2.ppm>\n");
}


static void
_compare(Col  *im1,
         Col  *im2,
         int   w1,
         int   h1,
         int   w2,
         int   h2,
         FILE *o)
{
   int i, j, idx;

   if (!o) o = stdout;

   if ((w1 != w2) && (h1 != h2))
     {
        fprintf(stderr, "*** PPM files have different sizes\n");
        return;
     }
   for (i = 0; i < h1; i++)
     {
        for (j = 0; j < w1; j++)
          {
             idx = i * w1 + j;
             if ((im1[idx].r != im2[idx].r) ||
                 (im1[idx].g != im2[idx].g) ||
                 (im1[idx].b != im2[idx].b))
               {
                  fprintf(o, "[%i][%i] 0x%x 0x%x 0x%x -> 0x%x 0x%x 0x%x\n",
                          i, j, im1[idx].r, im1[idx].g, im1[idx].b,
                          im2[idx].r, im2[idx].g, im2[idx].b);
               }
          }
     }
}

int
main(int    argc,
     char **argv)
{
   Col *im1, *im2;
   int w1, h1, w2, h2;

   if (argc != 3)
     {
        _usage(stderr);
        return 1;
     }

   im1 = ppm_parse(argv[1], &w1, &h1);
   im2 = ppm_parse(argv[2], &w2, &h2);

   if ((!im1) || (!im2))
     {
        free(im1);
        free(im2);
        return 2;
     }

   _compare(im1, im2, w1, h1, w2, h2, stdout);

   free(im1);
   free(im2);

   return 0;
}

