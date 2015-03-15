#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#include "pud_private.h"

bool
pud_jpeg_write(const char          *file,
               int                  w,
               int                  h,
               const unsigned char *data)
{
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   JSAMPROW row_pointer[1];
   int row_stride;
   FILE *f;
   unsigned char *data_rgb;
   int i, k, size;
   const JSAMPLE *p;

   data_rgb = malloc(sizeof(unsigned char) * 3 * w * h);
   if (!data_rgb) DIE_RETURN(false, "Failed to allocate rgb buffer");

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
   if (!f) DIE_RETURN(false, "Failed to open file [%s]", file);

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

   return true;
}

bool
pud_minimap_to_jpeg(Pud        *pud,
                    const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   unsigned char *map;
   bool chk;

   map = pud_minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = pud_jpeg_write(file, pud->map_w, pud->map_h, map);
   free(map);

   if (chk)
     PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return chk;
}

