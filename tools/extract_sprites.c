/*
 * sprites_colorize.c
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "ppm.h"
#include "pud_private.h"
#include "war2.h"
#include <Eet.h>

#define SIZEOF_ARRAY(arr_) (sizeof(arr_) / sizeof(*arr_))

static Eet_File *_ef = NULL;

static void *
_convert(const Pud_Color *sprite,
         int              x,
         int              y,
         int              w,
         int              h,
         int             *size_ret)
{
   const int size = w * h * sizeof(Pud_Color) + 8;
   unsigned char *data, tmp;
   int i;

   data = malloc(size);
   if (!data)
     {
        fprintf(stderr, "*** Failed to alloc size [%i]\n", size);
        return NULL;
     }

   /* Format:
    *  16bits: offset X of image
    *  16bits: offset Y of image
    *  16bits: width of image
    *  16bits: height of image
    *   Nbits: raw image */
   memcpy(data + 0, &x, 2);
   memcpy(data + 2, &y, 2);
   memcpy(data + 4, &w, 2);
   memcpy(data + 6, &h, 2);
   memcpy(data + 8, sprite, size - 4);

   /* Set ALPHA as the highest bit to fit the ARGB8888 colorspace
    * (Store as BGRA) for decoding */
   for (i = 8; i < size; i += 4)
     {
        tmp = data[i + 2];
        data[i + 2] = data[i + 0];
        data[i + 0] = tmp;
     }

   *size_ret = size;
   return data;
}

static void
_unit_cb(const Pud_Color               *sprite,
         int                            x,
         int                            y,
         int                            w,
         int                            h,
         const War2_Sprites_Descriptor *ud,
         int                            img_nb)
{
   void *data;
   int bytes, size;
   char era[16];
   char key[32];
   unsigned int u = ud->object;

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

   /* Get tileset */
   snprintf(era, sizeof(era), "%s", pud_era2str(ud->era));
   era[0] += 32; /* Lowercase, b*tch */

   data = _convert(sprite, x, y, w, h, &size);
   if (!data) return;

   /* Generate key */
   if ((u == PUD_UNIT_GNOMISH_SUBMARINE) ||
       (u == PUD_UNIT_GIANT_TURTLE))
     snprintf(key, sizeof(key), "%s/%s/%i", pud_unit2str(ud->object), era, img_nb);
   else
     snprintf(key, sizeof(key), "%s/%i", pud_unit2str(ud->object), img_nb);

   bytes = eet_write(_ef, key, data, size, 1);
   if (bytes <= 0)
     fprintf(stderr, "*** Failed to save key [%s]\n", key);

#if 0
   /* Quick and dirty debug */
   char nopath[128];
   snprintf(nopath, 128, "%s.png", key);
   for (int i = 0; i < (int)strlen(nopath); i++)
     if (nopath[i] == '/') nopath[i] = '_';
   war2_png_write(nopath, w, h, (unsigned char *)sprite);
#endif

   free(data);
}

static void
_building_cb(const Pud_Color               *sprite,
             int                            x,
             int                            y,
             int                            w,
             int                            h,
             const War2_Sprites_Descriptor *ud,
             int                            img_nb)
{
   void *data;
   int bytes, size;
   char key[32];

   /* Only handle the 5 first images [0,4] */
   if (img_nb > 0) return;

   data = _convert(sprite, x, y, w, h, &size);
   if (!data) return;

   snprintf(key, sizeof(key), "%s", pud_unit2str(ud->object));
   bytes = eet_write(_ef, key, data, size, 1);
   if (bytes <= 0)
     fprintf(stderr, "*** Failed to save key [%s]\n", key);

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

   free(data);
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
      PUD_UNIT_SKELETON,
      PUD_UNIT_DAEMON,
      PUD_UNIT_DWARVES,
      PUD_UNIT_GRYPHON_RIDER,
      PUD_UNIT_GNOMISH_FLYING_MACHINE,
      PUD_UNIT_HUMAN_TRANSPORT,
      PUD_UNIT_BATTLESHIP,
      /*PUD_UNIT_GNOMISH_SUBMARINE,*/ // Specific
      PUD_UNIT_INFANTRY,
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
      ud = war2_sprites_decode(w2, PUD_PLAYER_RED, era_, unit_, _unit_cb); \
      war2_sprites_descriptor_free(ud); \
   } while (0)

#define GEN_BUILDING(unit_, era_) \
   do { \
      ud = war2_sprites_decode(w2, PUD_PLAYER_RED, era_, unit_, _building_cb); \
      war2_sprites_descriptor_free(ud); \
   } while (0)

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <maindat.war>\n", argv[0]);
        return 1;
     }

   war2_init();
   eet_init();
   w2 = war2_open(argv[1], 2);
   if (!w2) return 2;


   /*=========================*
    *          UNITS          *
    *=========================*/
   _ef = eet_open("../data/sprites/units/units.eet", EET_FILE_MODE_WRITE);
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
   _ef = eet_open("../data/sprites/buildings/forest.eet", EET_FILE_MODE_WRITE);
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
   _ef = eet_open("../data/sprites/buildings/winter.eet", EET_FILE_MODE_WRITE);
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
   _ef = eet_open("../data/sprites/buildings/wasteland.eet", EET_FILE_MODE_WRITE);
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
   _ef = eet_open("../data/sprites/buildings/swamp.eet", EET_FILE_MODE_WRITE);
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

   printf("\n"
          "Do not forget to run cmake to copy generated files\n"
          "\n");

   return 0;
}

