/*
 * Copyright (c) 2017 Jean Guyomarc'h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "war2_private.h"


PUDAPI Pud_Color *
war2_cursors_decode(War2_Data *w2,
                    unsigned int entry,
                    int *x, int *y,
                    unsigned int *w,
                    unsigned int *h)
{
   unsigned char *mem;
   size_t size, img_size;
   uint16_t hotx, hoty, width, height;
   Pud_Color *img_rgba;
   unsigned int k;
   const Pud_Color *const palette = war2_palette_get(w2, PUD_ERA_FOREST);

   mem = war2_entry_extract(w2, entry, &size);
   if (! mem) DIE_RETURN(NULL, "Failed to extract entry");

   /*
    * x, y, w and h are encoded in the first 2*4 = 8 bytes of the entry.
    * What is left of the entry is the image data.
    */
   memcpy(&hotx, &(mem[0]), sizeof(uint16_t));
   memcpy(&hoty, &(mem[2]), sizeof(uint16_t));
   memcpy(&width, &(mem[4]), sizeof(uint16_t));
   memcpy(&height, &(mem[6]), sizeof(uint16_t));
   mem += 8;

   img_size = width * height;
   img_rgba = malloc(img_size * sizeof(Pud_Color));
   if (! img_rgba) DIE_GOTO(fail, "Failed to allocate memory");

   for (k = 0; k < img_size; k++)
     img_rgba[k] = palette[mem[k]];

   /* Free. Don't forget to go back of 8 bytes */
   mem -= 8;
   free(mem);

   if (x) *x = hotx;
   if (y) *y = hoty;
   if (w) *w = width;
   if (h) *h = height;
   return img_rgba;

fail:
   free(mem);
   return NULL;
}
