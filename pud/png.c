#include "pud_private.h"
#include <png.h>

bool
pud_png_write(const char          *file,
              int                  w,
              int                  h,
              const unsigned char *data)
{
   FILE *f;
   int i;
   png_structp png_ptr;
   png_infop info_ptr;
   png_bytepp row_pointers;

   f = fopen(file, "wb");
   if (!f) DIE_RETURN(false, "Failed to open [%s]", file);

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

   return true;

errf:
   png_destroy_write_struct(&png_ptr, &info_ptr);
err:
   fclose(f);
   return false;
}

bool
pud_minimap_to_png(Pud        *pud,
                   const char *file)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, false);

   unsigned char *map;
   bool chk;

   map = pud_minimap_bitmap_generate(pud, NULL);
   if (!map) DIE_RETURN(false, "Failed to generate bitmap");

   chk = pud_png_write(file, pud->map_w, pud->map_h, map);
   free(map);

   if (chk)
     PUD_VERBOSE(pud, 1, "Created [%s]", file);

   return chk;
}

