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

#include <pud.h>
#include <war2.h>

#include <cairo.h>
#include <Eina.h>
#include <Ecore_File.h>

#include <stdlib.h>
#include <string.h>

typedef struct _Sprite Sprite;

struct _Sprite
{
   int x;
   int y;
   unsigned int w;
   unsigned int h;
   Pud_Color *data;
   Sprite *next;
};

typedef struct
{
   Pud_Unit unit;
   Sprite sprites;
   unsigned int sprites_count;
   Eina_Tmpstr *file;
} Unit;

static void
_unit_sprite_add(Unit *u, int x, int y,
                 unsigned int w, unsigned int h,
                 const Pud_Color *data)
{
   const size_t size = w * h * sizeof(Pud_Color);
   Sprite *const s = malloc(sizeof(Sprite));

   s->x = x;
   s->y = y;
   s->w = w;
   s->h = h;
   s->data = malloc(size);
   memcpy(s->data, data, size);
   s->next = NULL;

   Sprite *iter = &u->sprites;
   while (iter->next) { iter = iter->next; }
   iter->next = s;
   u->sprites_count++;
}

static void
_unit_clean(Unit *u)
{
   Sprite *iter = u->sprites.next;
   while (iter)
     {
        Sprite *const next = iter->next;

        free(iter->data);
        free(iter);
        iter = next;
     }
}

static void
_unit_process(const Unit *u, unsigned int object)
{
   const Sprite *iter = u->sprites.next;
   char file[1024];
   cairo_surface_t *img, *im;
   cairo_t *cr;
   int fd;
   unsigned int row = 0, col = 0;

   unsigned int max_w = 0, max_h = 0;
   while (iter)
     {
        if (iter->w > max_w) max_w = iter->w;
        if (iter->h > max_h) max_h = iter->h;
        iter = iter->next;
     }

   unsigned int total_rows = u->sprites_count / 5;
   const unsigned int extra = u->sprites_count % 5;
   if (extra != 0) total_rows++;

   img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, max_w * 5, max_h * total_rows);
   cr = cairo_create(img);

   iter = u->sprites.next;
   while (iter)
     {
        war2_png_write(u->file, iter->w, iter->h, (unsigned char *)iter->data);
        im = cairo_image_surface_create_from_png(u->file);

        const double w = max_w * col;
        const double h = max_h * row;
        cairo_set_source_surface(cr, im, w, h);
        cairo_rectangle(cr, w, h, iter->w, iter->h);
        cairo_fill(cr);

        cairo_surface_destroy(im);
        iter = iter->next;
        col++;
        if (col >= 5)
          {
             col = 0;
             row++;
          }
     }

   snprintf(file, sizeof(file), "%s.png",
            pud_unit_to_string(object, PUD_FALSE));


   cairo_surface_flush(img);
   cairo_surface_write_to_png(img, file);

   cairo_destroy(cr);
   cairo_surface_destroy(img);
}

static void
_decode_sprite_cb(void                          *fdata,
                  const Pud_Color               *sprite,
                  int                            x,
                  int                            y,
                  unsigned int                   w,
                  unsigned int                   h,
                  const War2_Sprites_Descriptor *ud,
                  uint16_t                       img_nb)
{
   Unit *const unit = fdata;

   _unit_sprite_add(unit, x, y, w, h, sprite);
}

int
main(int                argc,
     const char *const *argv)
{
   int rc = EXIT_FAILURE;
   unsigned int object;

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <maindat.war>\n", argv[0]);
        goto end;
     }
   const char *const maindat = argv[1];

   pud_init();
   war2_init();

   War2_Data *const w2 = war2_open(maindat);
   if (! w2)
     {
        fprintf(stderr, "*** Failed to open '%s'\n", maindat);
        goto shutdown;
     }

   Eina_Tmpstr *file;
   const int fd = eina_file_mkstemp("tmp.sprites-XXXXXX.png", &file);

   for (object = 0; object < PUD_UNIT_NONE; object++)
     {
        Unit unit = {
           .unit = object,
           .sprites_count = 0,
           .sprites.next = NULL,
           .file = file,
        };
        if (war2_sprites_decode(w2, PUD_PLAYER_RED,
                                PUD_ERA_FOREST, object,
                                _decode_sprite_cb, &unit))
          {

             _unit_process(&unit, object);
          }
        _unit_clean(&unit);
     }

   close(fd);
   ecore_file_unlink(file);
   eina_tmpstr_del(file);

   rc = EXIT_SUCCESS;
close:
   war2_close(w2);
shutdown:
   war2_shutdown();
   pud_shutdown();
end:
   return rc;
}
