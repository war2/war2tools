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

#include <war2.h>

#include <cairo.h>
#include <Ecore_File.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

typedef struct
{
   cairo_surface_t *img;
   cairo_t *cr;
   char *png;
} Cairo_Ctx;

typedef struct
{
   Pud_Color *bitmap;
   uint16_t id;
} Icon;

typedef void (*Generator_Func)(void);

static Cairo_Ctx _cairo;
static Eina_List *_list = NULL;


#define ICON_W 46
#define ICON_H 38


static void
_add_icon(const Pud_Color *col,
          uint16_t id)
{
   Icon *const ic = malloc(sizeof(Icon));
   const size_t size = sizeof(Pud_Color) * ICON_W * ICON_H;
   ic->bitmap = malloc(size);
   memcpy(ic->bitmap, col, size);
   ic->id = id;

   _list = eina_list_append(_list, ic);
}

static void
_generate_atlas(unsigned int cols,
                unsigned int rows)
{
   Eina_List *l;
   Icon *ic;
   Eina_Tmpstr *file;
   unsigned int it = 0;
   unsigned int px, py;
   const int fd = eina_file_mkstemp("tmp.atlas-XXXXXX.png", &file);
   if (EINA_UNLIKELY(fd < 0))
     {
        fprintf(stderr, "*** Failed to create temporary file\n");
        return;
     }
   close(fd);

   EINA_LIST_FOREACH(_list, l, ic)
     {
        cairo_surface_t *img;

        war2_png_write(file, ICON_W, ICON_H, (unsigned char *)ic->bitmap);

        img = cairo_image_surface_create_from_png(file);

        cairo_set_source_surface(_cairo.cr, img, px, py);
        cairo_mask_surface(_cairo.cr, img, px, py);
        cairo_fill(_cairo.cr);
        cairo_surface_destroy(img);

        it++;
        if (it % cols == 0)
          {
             px = 0;
             py += ICON_H;
          }
        else
          {
             px += ICON_W;
          }
     }

   ecore_file_unlink(file);
   eina_tmpstr_del(file);
}

static void
_generate_lua()
{
}

static void
_open_era_file(const char *era, unsigned int cols, unsigned int rows)
{
   char my_getcwd[1024];
   char my_path2[1024];
   char my_path[1024];

   _cairo.img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                           cols * ICON_W,
                                           rows * ICON_H);
   _cairo.cr = cairo_create(_cairo.img);

   snprintf(my_path2, sizeof(my_path2),
            "%s/icons", getcwd(my_getcwd, sizeof(my_getcwd)));
   ecore_file_mkpath(my_path2);
   snprintf(my_path, sizeof(my_path), "%s/%s.png", my_path2, era);
   _cairo.png = strdup(my_path);
}

static void
_close_file(void)
{
   cairo_surface_write_to_png(_cairo.img, _cairo.png);
   cairo_destroy(_cairo.cr);
   cairo_surface_destroy(_cairo.img);
   free(_cairo.png);
}

static inline void
_usage(FILE *s)
{
   fprintf(s, "*** Usage: extract_icons <maindat.war> <c|lua>\n");
}


static void
_icons_cb(void                          *data EINA_UNUSED,
          const Pud_Color               *icon,
          int                            x    EINA_UNUSED,
          int                            y    EINA_UNUSED,
          unsigned int                   w,
          unsigned int                   h,
          const War2_Sprites_Descriptor *ts   EINA_UNUSED,
          uint16_t                       img_nb)
{
   if ((w != ICON_W) || (h != ICON_H)) return;
   _add_icon(icon, img_nb);
 }

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Sprites_Descriptor *ud;
   int ret = EXIT_FAILURE;
   char buf[1024];
   Pud_Bool chk;
   struct {
      const Pud_Era era;
      const char *const str;
   } const eras[] = {
        { PUD_ERA_FOREST,    "forest" },
        { PUD_ERA_WINTER,    "winter" },
        { PUD_ERA_WASTELAND, "wasteland" },
        { PUD_ERA_SWAMP,     "swamp"},
   };
   unsigned int i;
   Generator_Func gen = NULL;

   if (argc != 3)
     {
        _usage(stderr);
        return 1;
     }
   const char *const file = argv[1];
   const char *const generator = argv[2];

   if (!strcmp(generator, "lua"))
     {
     }
   else if (!strcmp(generator, "c"))
     {
     }
   else
     {
        fprintf(stderr, "*** Unknown generator '%s'\n", generator);
        return 1;
     }

   ecore_file_init();
   war2_init();

   w2 = war2_open(file);
   if (!w2) goto deinit;

   for (i = 0; i < EINA_C_ARRAY_LENGTH(eras); i++)
     {
        unsigned int rows, cols, count;

        chk = war2_sprites_decode(w2, PUD_PLAYER_RED, eras[i].era,
                                  WAR2_SPRITES_ICONS, _icons_cb, NULL);
        if (!chk)
          {
             fprintf(stderr, "*** Failed to decode tileset %s\n", eras[i].str);
             return 1;
          }
        count = eina_list_count(_list);

        cols = ceil(sqrt(count));
        rows = floor(sqrt(count));
        printf("cols, row = %u x %u\n", cols, rows);

        printf("count = %u\n", eina_list_count(_list));

        _open_era_file(eras[i].str, cols, rows);
        _generate_atlas(cols, rows);

        _list = eina_list_free(_list);
        _close_file();
     }

   printf("Output is in %s/icons/\n", getcwd(buf, sizeof(buf)));

   ret = EXIT_SUCCESS;

   war2_close(w2);
deinit:
   war2_shutdown();
   ecore_file_shutdown();

   return ret;
}
