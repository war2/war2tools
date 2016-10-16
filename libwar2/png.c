
/*
 * Copyright (c) 2015-2016 Jean Guyomarc'h
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

#if HAVE_PNG
# include <png.h>
#endif

PUDAPI Pud_Bool
war2_png_write(const char          *file,
               unsigned int          w,
               unsigned int          h,
               const unsigned char *data)
{
#if HAVE_PNG
   FILE *f;
   int i;
   png_structp png_ptr;
   png_infop info_ptr;
   png_bytepp row_pointers;

   f = fopen(file, "wb");
   if (!f) DIE_RETURN(PUD_FALSE, "Failed to open [%s]", file);

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr) DIE_GOTO(err, "Failed to create png struct");

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) DIE_GOTO(errf, "Failed to create png info struct");

   png_init_io(png_ptr, f);

   png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGBA,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                PNG_FILTER_TYPE_BASE);
   png_write_info(png_ptr, info_ptr);

   row_pointers = malloc(h * sizeof(unsigned char *));
   if (!row_pointers) DIE_GOTO(errf, "Failed to allocate memory");
   for (i = 0; i < h; i++)
     row_pointers[i] = (png_bytep)(&(data[i * w * 4]));

   png_write_image(png_ptr, row_pointers);
   png_write_end(png_ptr, NULL);
   png_destroy_write_struct(&png_ptr, &info_ptr);
   free(row_pointers);
   fclose(f);

   return PUD_TRUE;

errf:
   png_destroy_write_struct(&png_ptr, &info_ptr);
err:
   fclose(f);
   return PUD_FALSE;

#else
   (void) file;
   (void) w;
   (void) h;
   (void) data;
   return PUD_FALSE;
#endif
}
