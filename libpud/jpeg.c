#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#include "../include/bool.h"
#include "../include/jpeg.h"
#include "../include/debug.h"

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
   const JSAMPLE *p = data;

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

   return true;
}

