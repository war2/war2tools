
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

#if HAVE_JPEG
# include <jpeglib.h>
#endif

PUDAPI Pud_Bool
war2_jpeg_write(const char          *file,
                unsigned int         w,
                unsigned int         h,
                const unsigned char *data)
{
#if HAVE_JPEG
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   JSAMPROW row_pointer[1];
   int row_stride;
   FILE *f;
   unsigned char *data_rgb;
   int i, k, size;
   const JSAMPLE *p;

   data_rgb = malloc(sizeof(unsigned char) * 3 * w * h);
   if (!data_rgb) DIE_RETURN(PUD_FALSE, "Failed to allocate rgb buffer");

   /* data is RGBA. JPEG does not like alpha */
   size = w * h * 4;
   for (i = 0, k = 0; i < size; i += 4, k += 3)
     {
        data_rgb[k + 0] = data[i + 0];
        data_rgb[k + 1] = data[i + 1];
        data_rgb[k + 2] = data[i + 2];
     }
   p = data_rgb;

   f = fopen(file, "wb");
   if (!f)
     {
        free(data_rgb);
        DIE_RETURN(PUD_FALSE, "Failed to open file [%s]", file);
     }

   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_compress(&cinfo);
   jpeg_stdio_dest(&cinfo, f);

   cinfo.image_width = w;
   cinfo.image_height = h;
   cinfo.input_components = 3;
   cinfo.in_color_space = JCS_RGB;

   row_stride = w * 3;

   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, 100, TRUE);
   jpeg_start_compress(&cinfo, TRUE);

   while (cinfo.next_scanline < cinfo.image_height)
     {
        row_pointer[0] = (JSAMPROW)(&p[cinfo.next_scanline * row_stride]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
     }

   jpeg_finish_compress(&cinfo);
   fclose(f);
   jpeg_destroy_compress(&cinfo);
   free(data_rgb);

   return PUD_TRUE;
#else
   (void) file;
   (void) w;
   (void) h;
   (void) data;
   return PUD_FALSE;
#endif
}
