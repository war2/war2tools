/* font.c */

#include <stdlib.h>
#include "war2.h"
#include "debug.h"

struct font {
   Pud_Color **img;
   size_t img_width;
   size_t img_height;
   size_t max_width;

   size_t start_x;
   size_t start_y;
   size_t x_offset;
   size_t y_offset;

   size_t current_img_width;
};

static void _start(void *const data, const size_t nb_glyphs, const size_t max_width, const size_t max_height)
{
   struct font *const f = data;

   // We are starting at coords (0, 0)
   f->start_x = 0;
   f->start_y = 0;

   // No offset by default
   f->x_offset = 0;
   f->y_offset = 0;

   // Add 2 pixels to every width: we will add glyph separators
   // Cf https://love2d.org/wiki/ImageFontFormat
   f->img_width = nb_glyphs * (max_width + 2);
   f->img_height = max_height;

   f->max_width = max_width;

   // Use an Iliffe vector to allocate memory once and for all. This also allows to a
   // quick and easy image manipulation.
   f->img = malloc(sizeof(Pud_Color *) * f->img_height);
   f->img[0] = calloc(f->img_width * f->img_height, sizeof(Pud_Color));
   for (size_t i = 1; i < f->img_height; i++)
   { f->img[i] = f->img[i - 1] + f->img_width; }
}

static void _draw_separator(struct font *const f)
{
   for (size_t y = 0; y < f->img_height; y++)
   {
      f->img[y][f->start_x] = pud_color(0xff, 0x00, 0x00, 0xff); // Pure red
   }
   // Move the x-pointer to the right
   f->start_x++;
}

static void _glyph_start(void *data, uint8_t glyph, size_t width, size_t height, size_t x_offset, size_t y_offset)
{
   struct font *const f = data;
   _draw_separator(f);

   f->start_y = 0;
   f->x_offset = x_offset;
   f->y_offset = y_offset;
   f->current_img_width = width;
}

static void _glyph_end(void *data, uint8_t glyph)
{
   struct font *const f = data;
   // End of the glyph: we move the x-pointer to the end of the image.
   // Then we draw the separator, and we will be ready for the next glyph.
   f->start_x += f->current_img_width;
   _draw_separator(f);
}

static void _glyph_pixel(void *data, uint8_t glyph, size_t x, size_t y, Pud_Color color)
{
   struct font *const f = data;

   const size_t row = f->start_y + f->y_offset + y;
   const size_t col = f->start_x + f->x_offset + x;
   f->img[row][col] = color;
}

static void _glyph_empty(void *data, uint8_t glyph)
{
   struct font *const f = data;

   f->x_offset = 0;
   f->y_offset = 0;

   _draw_separator(f);

   for (size_t y = 0; y < f->img_height; y++)
   {
      for (size_t x = 0; x < f->max_width; x++)
      {
         _glyph_pixel(f, glyph, x, y, pud_color(0, 0, 0, 0));
      }
   }
   f->start_x += f->max_width;
   _draw_separator(f);
}

int main(const int argc, const char *const argv[])
{
   int status = EXIT_FAILURE;
   if (argc != 3)
   {
      fprintf(stderr, "*** Usage: %s <maindat.war> <output.png>\n", argv[0]);
      goto end;
   }
   const char *const file = argv[1];
   const char *const output = argv[2];

   War2_Data *const w2 = war2_open(file);
   if (! w2) { goto end; }


   struct font font;
   const War2_Font_Decoder font_decoder = {
      .start = &_start,
      .end = NULL,
      .glyph_start = &_glyph_start,
      .glyph_end = &_glyph_end,
      .glyph_pixel = &_glyph_pixel,
      .glyph_empty = &_glyph_empty,
      .data = &font,
   };
   war2_font_decode(w2, WAR2_FONT_TITLE_LARGE, &font_decoder);

   war2_png_write(output, (unsigned)font.img_width, (unsigned)font.img_height,
                  (const unsigned char *)font.img[0]);

   war2_close(w2);
   status = EXIT_SUCCESS;
end:
   return status;
}

