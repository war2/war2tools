#include <war2.h>
#include "../include/debug.h"

#define EXPORT_PNG 0
#define EXPORT_EET 1
#define EXPORT EXPORT_EET

#if !HAVE_EET && EXPORT == EXPORT_EET
# warning Cannot export as EET. Defaults to PNG.
# undef EXPORT
# define EXPORT EXPORT_PNG
#endif

#if EXPORT == EXPORT_EET
# include <Eet.h>
static Eet_File *_ef = NULL;
# define OPEN_EET(era) _ef = eet_open("../data/tiles/eet/" era ".eet", EET_FILE_MODE_READ_WRITE)
# define CLOSE_EET() eet_close(_ef)
#else
# define OPEN_EET(era)
# define CLOSE_EET()
#endif

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: extract <file.war> [dbg lvl = 0]\n");
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

#if EXPORT == EXPORT_PNG
static void
_export_tile(const Pud_Color    *tile,
             int                 w,
             int                 h,
             const War2_Tileset *ts,
             int                 img_nb)
{
   char buf[1024];

   /* Fog of war */
   if (img_nb <= 16) return;

   snprintf(buf, sizeof(buf), "../data/tiles/png/%s/%i.png",
            _era2str(ts->era), img_nb);

   pud_png_write(buf, w, h, (unsigned char *)tile);
}
#elif EXPORT == EXPORT_EET
static void
_export_tile(const Pud_Color    *tile,
             int                 w,
             int                 h,
             const War2_Tileset *ts,
             int                 img_nb)
{
   char key[8];
   int bytes;

   snprintf(key, sizeof(key), "%i", img_nb);
   bytes = eet_write(_ef, key, tile, w * h * sizeof(unsigned char) * 4, 1);
   if (bytes <= 0)
    fprintf(stderr, "*** Failed to save key [%s]", key);
}
#endif

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Tileset *ts;
   const char *file;
   int verbose;

   if (argc >= 2)
     {
        file = argv[1];
        verbose = (argc == 3) ? strtol(argv[2], NULL, 10) : 0;
     }
   else
     {
        _usage();
        return 1;
     }

#if EXPORT == EXPORT_EET
   eet_init();
#endif

   war2_init();
   w2 = war2_open(file, verbose);
   if (!w2) return 1;

   OPEN_EET("forest");
   ts = war2_tileset_decode(w2, PUD_ERA_FOREST, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset FOREST");
   war2_tileset_free(ts);
   CLOSE_EET();

   OPEN_EET("winter");
   ts = war2_tileset_decode(w2, PUD_ERA_WINTER, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WINTER");
   war2_tileset_free(ts);
   CLOSE_EET();

   OPEN_EET("wasteland");
   ts = war2_tileset_decode(w2, PUD_ERA_WASTELAND, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WASTELAND");
   war2_tileset_free(ts);
   CLOSE_EET();

   OPEN_EET("swamp");
   ts = war2_tileset_decode(w2, PUD_ERA_SWAMP, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset SWAMP");
   war2_tileset_free(ts);
   CLOSE_EET();

   war2_close(w2);
   war2_shutdown();

#if EXPORT == EXPORT_EET
   eet_shutdown();
#endif


   return 0;
}

