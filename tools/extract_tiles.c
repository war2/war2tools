/*
 * extract.c
 * extract_tiles
 *
 * Copyright (c) 2014 -2016 Jean Guyomarc'h
 */

#include "war2.h"
#include "debug.h"

#ifdef HAVE_EET
# include <Eet.h>
#endif

#ifdef HAVE_CAIRO
# include <cairo.h>
#endif

#include <Ecore_File.h>
#include <unistd.h>

typedef enum
{
   PNG,
   EET,
   ATLAS,
} Export_Type;

#define TILE_W 32
#define TILE_H 32

static Export_Type _export_type;

#ifdef HAVE_EET
static Eet_File *_ef = NULL;
#endif

#ifdef HAVE_CAIRO
typedef struct
{
   cairo_surface_t *img;
   cairo_t *cr;
   char *png;
} Cairo_Ctx;

static Cairo_Ctx _cairo;
#endif

static void
_open(const char *era)
{
   char my_getcwd[1024];
   char my_path2[1024];
   char my_path[1024];

   if (_export_type == EET)
     {
#ifdef HAVE_EET
        snprintf(my_path2, sizeof(my_path2),
                 "%s/tiles/eet", getcwd(my_getcwd, sizeof(my_getcwd)));
        ecore_file_mkpath(my_path2);
        snprintf(my_path, sizeof(my_path), "%s/%s.eet", my_path2, era);
        _ef = eet_open(my_path, EET_FILE_MODE_WRITE);
#endif
     }
   else if (_export_type == ATLAS)
     {
#ifdef HAVE_CAIRO
        int bytes;

        _cairo.img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                0x10 * TILE_H,
                                                14 * 9 * TILE_W + 0xc * TILE_W);
        _cairo.cr = cairo_create(_cairo.img);

        snprintf(my_path2, sizeof(my_path2),
                 "%s/tiles/atlas", getcwd(my_getcwd, sizeof(my_getcwd)));
        ecore_file_mkpath(my_path2);
        bytes = snprintf(my_path, sizeof(my_path), "%s/%s.png", my_path2, era);
        _cairo.png = strndup(my_path, bytes);
#endif
     }
}

static void
_close(void)
{
   if (_export_type == EET)
     {
#ifdef HAVE_EET
        eet_close(_ef);
#endif
     }
   else if (_export_type == ATLAS)
     {
        cairo_surface_write_to_png(_cairo.img, _cairo.png);
        cairo_destroy(_cairo.cr);
        cairo_surface_destroy(_cairo.img);
        free(_cairo.png);
     }
}

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: extract <maindat.war> <{eet,png,atlas}> [dbg lvl = 0]\n");
}

static inline const char *
_era2str(Pud_Era era)
{
   switch (era)
     {
      case PUD_ERA_FOREST:    return "forest";
      case PUD_ERA_WINTER:    return "winter";
      case PUD_ERA_WASTELAND: return "wasteland";
      case PUD_ERA_SWAMP:     return "swamp";
     }
   return "<ERROR>";
}

static void
_export_tile_png(const Pud_Color    *tile,
                 int                 w,
                 int                 h,
                 const War2_Tileset_Descriptor *ts,
                 int                 img_nb)
{
   char buf[1024], buf2[1024], buf3[1024];

   /* Fog of war */
   if (img_nb < 16) return;

   snprintf(buf, sizeof(buf),
            "%s/tiles/png/%s", getcwd(buf3, sizeof(buf3)), _era2str(ts->era));
   ecore_file_mkpath(buf);
   snprintf(buf2, sizeof(buf2), "%s/0x%04x.png", buf, img_nb);
   war2_png_write(buf2, w, h, (unsigned char *)tile);
}

#ifdef HAVE_CAIRO
static void
_export_tile_atlas(const Pud_Color *tile,
                   int w,
                   int h,
                   const War2_Tileset_Descriptor *ts EINA_UNUSED,
                   int img_nb)
{
   Eina_Tmpstr *file;
   int fd;
   int major, minor;
   int x, y;
   cairo_surface_t *img;
   const unsigned int solid_offset = (0xd + 1) * 0x9 * 32;

   /* Fog of war */
   if (img_nb < 16) return;

   fd = eina_file_mkstemp("tmp.atlas-XXXXXX.png", &file);
   if (EINA_UNLIKELY(fd < 0))
     {
        fprintf(stderr, "*** Failed to create temporary file\n");
        return;
     }
   close(fd);
   war2_png_write(file, w, h, (unsigned char *)tile);

   major = (img_nb & 0x0f00) >> 8;
   minor = (img_nb & 0x00f0) >> 4;
   if (major == 0) /* solid tiles */
     {
        y = solid_offset + ((minor - 1) * TILE_H);
        x = (img_nb & 0x000f) * TILE_W;
     }
   else
     {
        y = ((major - 1) * TILE_H * (0xd + 1)) + (minor * TILE_H);
        x = (img_nb & 0x000f) * TILE_W;
     }

   img = cairo_image_surface_create_from_png(file);

   cairo_set_source_surface(_cairo.cr, img, x, y);
   cairo_mask_surface(_cairo.cr, img, x, y);
   cairo_fill(_cairo.cr);
   cairo_surface_destroy(img);

   ecore_file_unlink(file);
   eina_tmpstr_del(file);
}
#endif

#ifdef HAVE_EET
static void
_export_tile_eet(const Pud_Color    *tile,
                 int                 w,
                 int                 h,
                 const War2_Tileset_Descriptor *ts EINA_UNUSED,
                 int                 img_nb)
{
   char key[8];
   unsigned char *data;
   int bytes, i;
   unsigned char tmp;
   const int size = sizeof(Pud_Color) * w * h;

   /* Fog of war */
   if (img_nb < 16) return;

   data = malloc(size);
   if (!data)
     {
        fprintf(stderr, "*** Failed to allocate data of size %i\n", size);
        return;
     }
   memcpy(data, tile, size);

   /* Set ALPHA as the highest bit to fit the ARGB8888 colorspace
    * (Store as BGRA) for decoding */
   for (i = 0; i < size; i += 4)
     {
        tmp = data[i + 2];
        data[i + 2] = data[i + 0];
        data[i + 0] = tmp;
     }

   snprintf(key, sizeof(key), "0x%04x", img_nb);
   bytes = eet_write(_ef, key, data, size, 1);
   if (bytes <= 0)
     fprintf(stderr, "*** Failed to save key [%s]\n", key);
   //printf("Write key [%s] in \"%s\"\n", key, eet_file_get(_ef));

   free(data);
}
#endif

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Tileset_Descriptor *ts;
   const char *file;
   const char *type;
   int verbose;
   int ret = EXIT_FAILURE;
   char buf[1024];
   War2_Tileset_Decode_Func func;
   const char *dest;

   if (argc >= 3)
     {
        file = argv[1];
        type = argv[2];
        if (!strcmp(type, "png"))
          _export_type = PNG;
        else if (!strcmp(type, "eet"))
          _export_type = EET;
        else if (!strcmp(type, "atlas"))
          _export_type = ATLAS;
        else
          {
             fprintf(stderr, "*** Invalid type \"%s\"\n", type);
             _usage();
             return 1;
          }
        verbose = (argc == 4) ? atoi(argv[3]) : 0;
     }
   else
     {
        _usage();
        return 1;
     }

   eet_init();
   ecore_file_init();
   war2_init();

   w2 = war2_open(file, verbose);
   if (!w2) return 1;

   switch (_export_type)
     {
      case EET:
         dest = "eet";
#ifdef HAVE_EET
         func = _export_tile_eet;
#else
         fprintf(stderr, "*** EET not supported\n");
         goto deinit;
#endif
         break;

      case ATLAS:
         dest = "atlas";
#ifdef HAVE_CAIRO
         func = _export_tile_atlas;
#else
         fprintf(stderr, "*** ATLAS not supported (no cairo)\n");
         goto deinit;
#endif
         break;

      case PNG:
      default:
         dest = "png";
         func = _export_tile_png;
         break;
     }


   _open("forest");
   ts = war2_tileset_decode(w2, PUD_ERA_FOREST, func);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset FOREST");
   war2_tileset_descriptor_free(ts);
   _close();

   _open("winter");
   ts = war2_tileset_decode(w2, PUD_ERA_WINTER, func);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WINTER");
   war2_tileset_descriptor_free(ts);
   _close();

   _open("wasteland");
   ts = war2_tileset_decode(w2, PUD_ERA_WASTELAND, func);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WASTELAND");
   war2_tileset_descriptor_free(ts);
   _close();

   _open("swamp");
   ts = war2_tileset_decode(w2, PUD_ERA_SWAMP, func);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset SWAMP");
   war2_tileset_descriptor_free(ts);
   _close();

   printf("Output is in %s/tiles/%s\n", getcwd(buf, sizeof(buf)), dest);

   ret = EXIT_SUCCESS;

   goto deinit; /* This is just here to silent a pointless warning */
deinit:
   war2_close(w2);
   war2_shutdown();
   ecore_file_shutdown();
   eet_shutdown();

   return ret;
}
