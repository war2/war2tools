/*
 * Copyright (c) Jean Guyomarc'h
 */

#include "ppm.h"
#include "war2.h"

#include <Eet.h>
#include <cairo.h>
#include <Ecore_File.h>
#include <unistd.h>

#define SIZEOF_ARRAY(arr_) (sizeof(arr_) / sizeof(*arr_))

static Eet_File *_ef = NULL;

static void
_unit_cb(void                          *fdata  EINA_UNUSED,
         const Pud_Color               *sprite,
         int                            x      EINA_UNUSED,
         int                            y      EINA_UNUSED,
         unsigned int                   w,
         unsigned int                   h,
         const War2_Sprites_Descriptor *ud,
         uint16_t                       img_nb)
{
   static const Pud_Unit aliases[] = {
      PUD_UNIT_UTHER_LIGHTBRINGER,      PUD_UNIT_KNIGHT,
      PUD_UNIT_LOTHAR,                  PUD_UNIT_KNIGHT,
      PUD_UNIT_TURALYON,                PUD_UNIT_KNIGHT,
      PUD_UNIT_ALLERIA,                 PUD_UNIT_ARCHER,
      PUD_UNIT_DANATH,                  PUD_UNIT_FOOTMAN,
      PUD_UNIT_KHADGAR,                 PUD_UNIT_MAGE,
      PUD_UNIT_KURDRAN_AND_SKY_REE,     PUD_UNIT_GRYPHON_RIDER,
      PUD_UNIT_GROM_HELLSCREAM,         PUD_UNIT_GRUNT,
      PUD_UNIT_KARGATH_BLADEFIST,       PUD_UNIT_GRUNT,
      PUD_UNIT_ZUL_JIN,                 PUD_UNIT_AXETHROWER,
      PUD_UNIT_CHO_GALL,                PUD_UNIT_OGRE,
      PUD_UNIT_DENTARG,                 PUD_UNIT_OGRE,
      PUD_UNIT_GUL_DAN,                 PUD_UNIT_DEATH_KNIGHT,
      PUD_UNIT_TERON_GOREFIEND,         PUD_UNIT_DEATH_KNIGHT,
      PUD_UNIT_DEATHWING,               PUD_UNIT_DRAGON,
      PUD_UNIT_ATTACK_PEASANT,          PUD_UNIT_PEASANT,
      PUD_UNIT_ATTACK_PEON,             PUD_UNIT_PEON,
   };
   static const unsigned int aliases_count = SIZEOF_ARRAY(aliases);

   void *data;
   int bytes;
   char key[64], key2[64];
   const Pud_Unit u = ud->object;
   unsigned int i;
   int fd;
   const int compress = 1;
   Eina_Bool ok;
   cairo_surface_t *img;
   Eina_Tmpstr *file;
   int cw, ch;

   /* Only handle the 5 first images [0,4] */
   if ((u == PUD_UNIT_HUMAN_START) ||
       (u == PUD_UNIT_ORC_START))
     {
        if (img_nb > 0)
          return;
     }
   else
     {
        if (img_nb > 4)
          return;
     }

   fd = eina_file_mkstemp("tmp.sprites-XXXXXX.png", &file);
   if (EINA_UNLIKELY(fd < 0))
     {
        fprintf(stderr, "*** Failed to create temporary file\n");
        return;
     }
   close(fd);

   war2_png_write(file, w, h, (unsigned char *)sprite);

   img = cairo_image_surface_create_from_png(file);
   cairo_surface_flush(img);
   data = cairo_image_surface_get_data(img);
   cw = cairo_image_surface_get_width(img);
   ch = cairo_image_surface_get_height(img);
   printf("format is %i\n" ,cairo_image_surface_get_format (img));

   /* Generate key */
   switch (u)
     {
      case PUD_UNIT_GNOMISH_SUBMARINE:
      case PUD_UNIT_GIANT_TURTLE:
         snprintf(key, sizeof(key), "%s/%s/%i",
                  pud_unit_to_string(ud->object, PUD_FALSE),
                  pud_era_to_string(ud->era), img_nb);
         break;

      case PUD_UNIT_CRITTER_SHEEP:
         snprintf(key, sizeof(key), "critter/Forest/%i", img_nb);
         break;

      case PUD_UNIT_CRITTER_PIG:
         snprintf(key, sizeof(key), "critter/Wasteland/%i", img_nb);
         break;

      case PUD_UNIT_CRITTER_SEAL:
         snprintf(key, sizeof(key), "critter/Winter/%i", img_nb);
         break;

      case PUD_UNIT_CRITTER_RED_PIG:
         snprintf(key, sizeof(key), "critter/Swamp/%i", img_nb);
         break;

      default:
         snprintf(key, sizeof(key), "%s/%i", pud_unit_to_string(u, PUD_FALSE), img_nb);
         break;
     }

   bytes = eet_data_image_write(_ef, key, data, cw, ch, 1, compress, 100, 0);
   if (bytes <= 0)
     fprintf(stderr, "*** Failed to save key [%s]\n", key);

   /* Generate aliases */
   for (i = 0; i < aliases_count; i += 2)
     {
        if (u == aliases[i + 1])
          {
             snprintf(key2, sizeof(key2), "%s/%i",
                      pud_unit_to_string(aliases[i], PUD_FALSE), img_nb);
             ok = eet_alias(_ef, key2, key, compress);
             if (!ok)
               {
                  fprintf(stderr, "*** Failed to set alias: [%s]->[%s]\n",
                          key2, key);
               }
          }
     }

#if 0
   /* Quick and dirty debug */
   char nopath[128];
   snprintf(nopath, 128, "%s.png", key);
   for (i = 0; i < strlen(nopath); i++)
     if (nopath[i] == '/') nopath[i] = '_';
   war2_png_write(nopath, w, h, (unsigned char *)sprite);
#endif

   cairo_surface_destroy(img);
   ecore_file_unlink(file);
   eina_tmpstr_del(file);
}

static void
_building_cb(void                          *fdata  EINA_UNUSED,
             const Pud_Color               *sprite,
             int                            x      EINA_UNUSED,
             int                            y      EINA_UNUSED,
             unsigned int                   w,
             unsigned int                   h,
             const War2_Sprites_Descriptor *ud,
             uint16_t                       img_nb)
{
   void *data;
   int bytes;
   char key[32];
   cairo_surface_t *img;
   Eina_Tmpstr *file;
   int fd, cw, ch;
   const int compress = 1;

   /* Only handle the first image */
   if (img_nb > 0) return;

   fd = eina_file_mkstemp("tmp.sprites-XXXXXX.png", &file);
   if (EINA_UNLIKELY(fd < 0))
     {
        fprintf(stderr, "*** Failed to create temporary file\n");
        return;
     }
   close(fd);

   war2_png_write(file, w, h, (unsigned char *)sprite);

   img = cairo_image_surface_create_from_png(file);
   cairo_surface_flush(img);
   data = cairo_image_surface_get_data(img);
   cw = cairo_image_surface_get_width(img);
   ch = cairo_image_surface_get_height(img);


   snprintf(key, sizeof(key), "%s/%s",
            pud_era_to_string(ud->era),
            pud_unit_to_string(ud->object, PUD_FALSE));
   bytes = eet_data_image_write(_ef, key, data, cw, ch, 1, compress, 100, 0);
   if (bytes <= 0)
     fprintf(stderr, "*** Failed to save key [%s]\n", key);

   cairo_surface_destroy(img);
   ecore_file_unlink(file);
   eina_tmpstr_del(file);

#if 0
   /* Quick and dirty debug */
   char nopath[128];
   char era[16];
   snprintf(era, sizeof(era), "%s", pud_era2str(ud->era));
   era[0] += 32; /* Lowercase, b*tch */
   snprintf(nopath, 128, "%s_%s.png", key, era);
   for (int i = 0; i < (int)strlen(nopath); i++)
     if (nopath[i] == '/') nopath[i] = '_';
   war2_png_write(nopath, w, h, (unsigned char *)sprite);
#endif
}

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Sprites_Descriptor *ud;
   unsigned int i;

   /* Heroes have the same sprites than standard units.
    * Don't need to load them. */

   const Pud_Unit units[] = {
      PUD_UNIT_GOBLIN_SAPPER,
      PUD_UNIT_DRAGON,
      PUD_UNIT_ORC_TRANSPORT,
      PUD_UNIT_JUGGERNAUGHT,
      PUD_UNIT_ORC_TANKER,
      /*PUD_UNIT_GIANT_TURTLE,*/ // Specific
      PUD_UNIT_GRUNT,
      PUD_UNIT_PEON,
      PUD_UNIT_CATAPULT,
      PUD_UNIT_OGRE,
      /*PUD_UNIT_EYE_OF_KILROGG,*/ // Created by a spell
      PUD_UNIT_AXETHROWER,
      PUD_UNIT_DEATH_KNIGHT,
      PUD_UNIT_ORC_TRANSPORT,
      PUD_UNIT_TROLL_DESTROYER,
      PUD_UNIT_GOBLIN_ZEPPLIN,
      PUD_UNIT_CRITTER_SHEEP,
      PUD_UNIT_CRITTER_PIG,
      PUD_UNIT_CRITTER_SEAL,
      PUD_UNIT_CRITTER_RED_PIG,
      PUD_UNIT_SKELETON,
      PUD_UNIT_DAEMON,
      PUD_UNIT_DWARVES,
      PUD_UNIT_GRYPHON_RIDER,
      PUD_UNIT_GNOMISH_FLYING_MACHINE,
      PUD_UNIT_HUMAN_TRANSPORT,
      PUD_UNIT_BATTLESHIP,
      /*PUD_UNIT_GNOMISH_SUBMARINE,*/ // Specific
      PUD_UNIT_FOOTMAN,
      PUD_UNIT_PEASANT,
      PUD_UNIT_BALLISTA,
      PUD_UNIT_KNIGHT,
      PUD_UNIT_ARCHER,
      PUD_UNIT_MAGE,
      PUD_UNIT_HUMAN_TANKER,
      PUD_UNIT_ELVEN_DESTROYER,
      PUD_UNIT_HUMAN_START,
      PUD_UNIT_ORC_START
   };
   const Pud_Unit buildings[] = {
      PUD_UNIT_HUMAN_GUARD_TOWER,
      PUD_UNIT_HUMAN_CANNON_TOWER,
      PUD_UNIT_MAGE_TOWER,
      PUD_UNIT_KEEP,
      PUD_UNIT_GRYPHON_AVIARY,
      PUD_UNIT_GNOMISH_INVENTOR,
      PUD_UNIT_FARM,
      PUD_UNIT_HUMAN_BARRACKS,
      PUD_UNIT_CHURCH,
      PUD_UNIT_HUMAN_SCOUT_TOWER,
      PUD_UNIT_TOWN_HALL,
      PUD_UNIT_ELVEN_LUMBER_MILL,
      PUD_UNIT_STABLES,
      PUD_UNIT_HUMAN_BLACKSMITH,
      PUD_UNIT_HUMAN_SHIPYARD,
      PUD_UNIT_HUMAN_FOUNDRY,
      PUD_UNIT_HUMAN_REFINERY,
      PUD_UNIT_HUMAN_OIL_WELL,
      PUD_UNIT_CASTLE,
      PUD_UNIT_ORC_GUARD_TOWER,
      PUD_UNIT_ORC_CANNON_TOWER,
      PUD_UNIT_TEMPLE_OF_THE_DAMNED,
      PUD_UNIT_STRONGHOLD,
      PUD_UNIT_DRAGON_ROOST,
      PUD_UNIT_GOBLIN_ALCHEMIST,
      PUD_UNIT_PIG_FARM,
      PUD_UNIT_ORC_BARRACKS,
      PUD_UNIT_ALTAR_OF_STORMS,
      PUD_UNIT_ORC_SCOUT_TOWER,
      PUD_UNIT_GREAT_HALL,
      PUD_UNIT_TROLL_LUMBER_MILL,
      PUD_UNIT_OGRE_MOUND,
      PUD_UNIT_ORC_BLACKSMITH,
      PUD_UNIT_ORC_SHIPYARD,
      PUD_UNIT_ORC_FOUNDRY,
      PUD_UNIT_ORC_REFINERY,
      PUD_UNIT_ORC_OIL_WELL,
      PUD_UNIT_FORTRESS,
      PUD_UNIT_OIL_PATCH,
      PUD_UNIT_GOLD_MINE,
      PUD_UNIT_DARK_PORTAL,
      PUD_UNIT_RUNESTONE,
      PUD_UNIT_CIRCLE_OF_POWER
   };

#define GEN_UNIT(unit_, era_) \
   do { \
      war2_sprites_decode(w2, PUD_PLAYER_RED, era_, unit_, _unit_cb, NULL); \
   } while (0)

#define GEN_BUILDING(unit_, era_) \
   do { \
      war2_sprites_decode(w2, PUD_PLAYER_RED, era_, unit_, _building_cb, NULL); \
   } while (0)

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <maindat.war>\n", argv[0]);
        return 1;
     }

   war2_init();
   eet_init();
   w2 = war2_open(argv[1]);
   if (!w2) return 2;
   war2_verbosity_set(w2, 2);


   /*=========================*
    *          UNITS          *
    *=========================*/
   _ef = eet_open("units.eet", EET_FILE_MODE_WRITE);
   if (!_ef)
     {
        war2_close(w2);
        war2_shutdown();
        fprintf(stderr, "*** Failed to open EET file for units\n");
        return -1;
     }
   for (i = 0; i < SIZEOF_ARRAY(units); i++)
     GEN_UNIT(units[i], PUD_ERA_FOREST);

   GEN_UNIT(PUD_UNIT_GNOMISH_SUBMARINE, PUD_ERA_FOREST);
   GEN_UNIT(PUD_UNIT_GNOMISH_SUBMARINE, PUD_ERA_WINTER);
   GEN_UNIT(PUD_UNIT_GNOMISH_SUBMARINE, PUD_ERA_WASTELAND);
   GEN_UNIT(PUD_UNIT_GNOMISH_SUBMARINE, PUD_ERA_SWAMP);
   GEN_UNIT(PUD_UNIT_GIANT_TURTLE, PUD_ERA_FOREST);
   GEN_UNIT(PUD_UNIT_GIANT_TURTLE, PUD_ERA_WINTER);
   GEN_UNIT(PUD_UNIT_GIANT_TURTLE, PUD_ERA_WASTELAND);
   GEN_UNIT(PUD_UNIT_GIANT_TURTLE, PUD_ERA_SWAMP);
   eet_close(_ef);

   /*=========================*
    *     FOREST BUILDINGS    *
    *=========================*/
   _ef = eet_open("forest.eet", EET_FILE_MODE_WRITE);
   if (!_ef)
     {
        war2_close(w2);
        war2_shutdown();
        fprintf(stderr, "*** Failed to open EET file for forest buildings\n");
        return -1;
     }
   for (i = 0; i < SIZEOF_ARRAY(buildings); i++)
     GEN_BUILDING(buildings[i], PUD_ERA_FOREST);
   eet_close(_ef);

   /*=========================*
    *     WINTER BUILDINGS    *
    *=========================*/
   _ef = eet_open("winter.eet", EET_FILE_MODE_WRITE);
   if (!_ef)
     {
        war2_close(w2);
        war2_shutdown();
        fprintf(stderr, "*** Failed to open EET file for winter buildings\n");
        return -1;
     }
   for (i = 0; i < SIZEOF_ARRAY(buildings); i++)
     GEN_BUILDING(buildings[i], PUD_ERA_WINTER);
   eet_close(_ef);

   /*=========================*
    *   WASTELAND BUILDINGS   *
    *=========================*/
   _ef = eet_open("wasteland.eet", EET_FILE_MODE_WRITE);
   if (!_ef)
     {
        war2_close(w2);
        war2_shutdown();
        fprintf(stderr, "*** Failed to open EET file for wasteland buildings\n");
        return -1;
     }
   for (i = 0; i < SIZEOF_ARRAY(buildings); i++)
     GEN_BUILDING(buildings[i], PUD_ERA_WASTELAND);
   eet_close(_ef);

   /*=========================*
    *      SWAMP BUILDINGS    *
    *=========================*/
   _ef = eet_open("swamp.eet", EET_FILE_MODE_WRITE);
   if (!_ef)
     {
        war2_close(w2);
        war2_shutdown();
        fprintf(stderr, "*** Failed to open EET file for swamp buildings\n");
        return -1;
     }
   for (i = 0; i < SIZEOF_ARRAY(buildings); i++)
     GEN_BUILDING(buildings[i], PUD_ERA_SWAMP);
   eet_close(_ef);

   war2_close(w2);
   eet_shutdown();
   war2_shutdown();

   return 0;
}
