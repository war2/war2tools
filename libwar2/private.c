/*
 * private.c
 * libwar2
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "war2_private.h"

Pud_Bool
war2_mem_map_ok(War2_Data *w2)
{
   return (w2->ptr < w2->mem_map + w2->mem_map_size);
}

void
war2_palette_convert(unsigned char *ptr,
                     Pud_Color      palette[256])
{
   unsigned char *p;
   unsigned int i;

   /* I don't know why this is the bitshift needed (no doc so no explaination)
    * but this gives the right colorspace (thanks wargus) */
   for (i = 0; i < 256; ++i)
     {
        p = &(ptr[i * 3]);
        palette[i].r = p[0] << 2;
        palette[i].g = p[1] << 2;
        palette[i].b = p[2] << 2;
        palette[i].a = 0xff;
     }
}

