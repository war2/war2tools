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
war2_ui_decode(War2_Data *w2,
               unsigned int entry,
               unsigned int *w,
               unsigned int *h)
{
   unsigned char *ptr;
   size_t size;
   uint16_t width, height;
   unsigned int img_size;
   unsigned int i;
   Pud_Color *img;
   const Pud_Color *const palette = war2_palette_get(w2, PUD_ERA_FOREST);

   ptr = war2_entry_extract(w2, entry, &size);
   if (! ptr) DIE_RETURN(NULL, "Failed to extract entry");

   memcpy(&width, &ptr[0], sizeof(uint16_t));
   memcpy(&height, &ptr[2], sizeof(uint16_t));

   img_size = width * height;
   img = malloc(img_size * sizeof(Pud_Color));
   if (! img)
     {
        free(ptr);
        DIE_RETURN(NULL, "Failed to allocate memory");
     }

   ptr += 4;
   for (i = 0; i < img_size; i++)
     {
       img[i] = palette[ptr[i]]; 
     }
   free(ptr - 4);

   if (w) *w = width;
   if (h) *h = height;
   return img;
}
