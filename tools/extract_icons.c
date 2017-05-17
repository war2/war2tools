/*
 * extract_icons.c
 * extract_icons
 *
 * Copyright (c) 2016 Jean Guyomarc'h
 */

#include "war2.h"
#include "debug.h"

#include <Eet.h>
#include <cairo.h>
#include <Ecore_File.h>
#include <unistd.h>

typedef struct
{
   cairo_surface_t *img;
   cairo_t *cr;
   char *png;
} Cairo_Ctx;

static Cairo_Ctx _cairo;

#define ICON_W 46
#define ICON_H 38

static void
_open_era_file(const char *era)
{
   char my_getcwd[1024];
   char my_path2[1024];
   char my_path[1024];

   _cairo.img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                           ICON_W,
                                           196 * ICON_H);
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
   fprintf(s, "*** Usage: extract_icons <maindat.war>\n");
}

static void
_icons_cb(void                          *data EINA_UNUSED,
          const Pud_Color               *tile,
          int                            x EINA_UNUSED,
          int                            y EINA_UNUSED,
          unsigned int                   w,
          unsigned int                   h,
          const War2_Sprites_Descriptor *ts EINA_UNUSED,
          uint16_t                       img_nb)
{
   Eina_Tmpstr *file;
   int fd;
   const int px = 0, py = img_nb * ICON_H;
   cairo_surface_t *img;

   if ((w != ICON_W) || (h != ICON_H)) return;

   fd = eina_file_mkstemp("tmp.atlas-XXXXXX.png", &file);
   if (EINA_UNLIKELY(fd < 0))
     {
        fprintf(stderr, "*** Failed to create temporary file\n");
        return;
     }
   close(fd);
   war2_png_write(file, w, h, (unsigned char *)tile);

   img = cairo_image_surface_create_from_png(file);

   cairo_set_source_surface(_cairo.cr, img, px, py);
   cairo_mask_surface(_cairo.cr, img, px, py);
   cairo_fill(_cairo.cr);
   cairo_surface_destroy(img);

   ecore_file_unlink(file);
   eina_tmpstr_del(file);
}

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Sprites_Descriptor *ud;
   const char *file;
   int ret = EXIT_FAILURE;
   char buf[1024];
   Pud_Bool chk;
   struct {
      Pud_Era era;
      const char *str;
   } const eras[] = {
        { PUD_ERA_FOREST,    "forest" },
        { PUD_ERA_WINTER,    "winter" },
        { PUD_ERA_WASTELAND, "wasteland" },
        { PUD_ERA_SWAMP,     "swamp"},
   };
   unsigned int i;

   if (argc != 2)
     {
        _usage(stderr);
        return 1;
     }

   file = argv[1];

   eet_init();
   ecore_file_init();
   war2_init();

   w2 = war2_open(file);
   if (!w2) goto deinit;

   for (i = 0; i < EINA_C_ARRAY_LENGTH(eras); i++)
     {
        _open_era_file(eras[i].str);
        chk = war2_sprites_decode(w2, PUD_PLAYER_RED, eras[i].era,
                                 WAR2_SPRITES_ICONS, _icons_cb, NULL);
        if (!chk) DIE_RETURN(2, "Failed to decode tileset %s", eras[i].str);
        _close_file();
     }

   printf("Output is in %s/icons/\n", getcwd(buf, sizeof(buf)));

   ret = EXIT_SUCCESS;

   war2_close(w2);
deinit:
   war2_shutdown();
   ecore_file_shutdown();
   eet_shutdown();

   return ret;
}
