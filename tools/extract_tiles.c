/*
 * extract.c
 * extract
 *
 * Copyright (c) 2014 -2015 Jean Guyomarc'h
 */

#include <war2.h>
#include "../include/debug.h"
#include <Eet.h>
#include <Ecore_File.h>
#include <unistd.h>

typedef enum
{
   PNG,
   EET
} Export_Type;

static Eet_File *_ef = NULL;
static Export_Type _export_type;

#define OPEN_EET(era) \
   do { \
      if (_export_type == EET) { \
         char my_getcwd__[1024]; \
         char my_path__[1024]; \
         char my_path2__[1024]; \
         snprintf(my_path2__, sizeof(my_path__), \
                  "%s/tiles/eet", getcwd(my_getcwd__, sizeof(my_getcwd__))); \
         ecore_file_mkpath(my_path2__); \
         snprintf(my_path__, sizeof(my_path__), "%s/%s.eet", \
                  my_path2__, era); \
         _ef = eet_open(my_path__, EET_FILE_MODE_WRITE); \
      } \
   } while (0)

#define CLOSE_EET() \
   do { \
      if (_export_type == EET) { \
         eet_close(_ef); \
      } \
   } while (0)

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: extract <maindat.war> <{eet,png}> [dbg lvl = 0]\n");
}

static const char *
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
   if (img_nb <= 16) return;

   snprintf(buf, sizeof(buf),
            "%s/tiles/png/%s", getcwd(buf3, sizeof(buf3)), _era2str(ts->era));
   ecore_file_mkpath(buf);
   snprintf(buf2, sizeof(buf2), "%s/0x%04x.png", buf, img_nb);
   war2_png_write(buf2, w, h, (unsigned char *)tile);
}

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

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Tileset_Descriptor *ts;
   const char *file;
   const char *type;
   int verbose;
   char buf[1024];

   if (argc >= 3)
     {
        file = argv[1];
        type = argv[2];
        if (!strcmp(type, "png"))
          _export_type = PNG;
        else if (!strcmp(type, "eet"))
          _export_type = EET;
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

#define EXPORT_FUNCTION() \
   (_export_type == EET) ? _export_tile_eet : _export_tile_png

   OPEN_EET("forest");
   ts = war2_tileset_decode(w2, PUD_ERA_FOREST, EXPORT_FUNCTION());
   if (!ts) DIE_RETURN(2, "Failed to decode tileset FOREST");
   war2_tileset_descriptor_free(ts);
   CLOSE_EET();

   OPEN_EET("winter");
   ts = war2_tileset_decode(w2, PUD_ERA_WINTER, EXPORT_FUNCTION());
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WINTER");
   war2_tileset_descriptor_free(ts);
   CLOSE_EET();

   OPEN_EET("wasteland");
   ts = war2_tileset_decode(w2, PUD_ERA_WASTELAND, EXPORT_FUNCTION());
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WASTELAND");
   war2_tileset_descriptor_free(ts);
   CLOSE_EET();

   OPEN_EET("swamp");
   ts = war2_tileset_decode(w2, PUD_ERA_SWAMP, EXPORT_FUNCTION());
   if (!ts) DIE_RETURN(2, "Failed to decode tileset SWAMP");
   war2_tileset_descriptor_free(ts);
   CLOSE_EET();

   war2_close(w2);

   war2_shutdown();
   ecore_file_shutdown();
   eet_shutdown();

   printf("Output is in %s/tiles/%s\n", getcwd(buf, sizeof(buf)),
          (_export_type == EET) ? "eet" : "png");

   return 0;
}

