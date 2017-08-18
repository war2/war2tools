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
   const char *path;
} Unit;

typedef struct
{
   const uint16_t section;
   const char *const name;
} Animation;

static const Animation _animations[] =
{
   { 352, "action", },
   { 0, NULL },
};

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
_draw(const Unit *u,
      const Sprite *s,
      cairo_t *cr,
      unsigned int col, unsigned int row,
      unsigned int max_w, unsigned int max_h,
      int can_flip)
{
   cairo_matrix_t mat;
   cairo_surface_t *im;
   const int flip = ((col == 5) || (col == 6) || (col == 7));
   const double w = max_w * col;
   const double h = max_h * row;
   const double at_x = max_w * col;

   if (flip)
     {
        cairo_matrix_init(&mat,
                          -1.0               , 0.0,
                          0.0                , 1.0,
                          (2.0 * at_x) + s->w, 0.0);
        cairo_save(cr);
        cairo_transform(cr, &mat);
     }

   war2_png_write(u->file, s->w, s->h, (unsigned char *)s->data);
   im = cairo_image_surface_create_from_png(u->file);


   cairo_set_source_surface(cr, im, w, h);
   cairo_rectangle(cr, w, h, s->w, s->h);
   cairo_fill(cr);

   if (flip)
     {
        cairo_restore(cr);
     }

   if ((can_flip) && ((col == 1) || (col == 2) || (col == 3)))
     {
        _draw(u, s, cr, 8 - col, row, max_w, max_h, 0);
     }

   cairo_surface_destroy(im);
}

static void
_common_process(const Unit *u, unsigned int cols, unsigned int add)
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

   unsigned int total_rows = u->sprites_count / cols;
   const unsigned int extra = u->sprites_count % cols;
   if (extra != 0) total_rows++;

   img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                    max_w * (cols + add),
                                    max_h * total_rows);
   cr = cairo_create(img);

   iter = u->sprites.next;
   while (iter)
     {
        _draw(u, iter, cr, col, row, max_w, max_h, (add != 0));
        col++;
        if (col >= cols)
          {
             col = 0;
             row++;
          }

        iter = iter->next;
     }

   snprintf(file, sizeof(file), "%s/%s.png",
            u->path, pud_unit_to_string(u->unit, PUD_FALSE));
   printf("=> %s\n", file);


   cairo_surface_flush(img);
   cairo_surface_write_to_png(img, file);

   cairo_destroy(cr);
   cairo_surface_destroy(img);
}

static void
_animation_process(const Unit *u)
{
   _common_process(u, u->sprites_count, 0);
}

static void
_unit_process(const Unit *u)
{
   _common_process(u, 5, 3);
}

static void
_building_process(const Unit *u)
{
   _common_process(u, 3, 0);
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
   unsigned int object, era, color;
   War2_Sprites_Decode_Func func;
   char path[4096];

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


   //for (era = PUD_ERA_FOREST; era <= PUD_ERA_SWAMP; era++)
   era = PUD_ERA_FOREST;
     {
        //for (color = 1; color <= 8; color++)
        color = 1;
          {
             snprintf(path, sizeof(path), "%s/%s/%u/",
                      (pud_unit_building_is(object) ? "buildings" : "units"),
                      pud_era_to_string(era), color);

             ecore_file_mkpath(path);
             for (object = 0; object < PUD_UNIT_NONE; object++)
            // object = PUD_UNIT_FOOTMAN;
               {
                  Unit unit = {
                     .unit = object,
                     .sprites_count = 0,
                     .sprites.next = NULL,
                     .file = file,
                     .path = path,
                  };

                  if (war2_sprites_decode(w2, color - 1, era, object,
                                          _decode_sprite_cb, &unit))
                    {
                       if (pud_unit_building_is(object))
                         {
                            _building_process(&unit);
                         }
                       else
                         {
                            _unit_process(&unit);
                         }
                    }
                  _unit_clean(&unit);
               }
          }
     }

   const Animation *a;
   for (a = _animations; a->section != 0; a++)
     {
        Unit unit = { /* This is a fake 'unit'. Bad name. */
           .unit = WAR2_SPRITES_ICONS,
           .sprites_count = 0,
           .sprites.next = NULL,
           .file = file,
           .path = "animations",
        };
        ecore_file_mkpath(unit.path);
        war2_sprites_decode_entry(w2, PUD_PLAYER_RED, a->section,
                                  _decode_sprite_cb, &unit);
        _animation_process(&unit);
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
