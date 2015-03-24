/*
 * units.c
 * libwar2
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "war2_private.h"

#define RLE_REPEAT (1 << 6)
#define RLE_LEAVE  (1 << 7)
#define PALETTE_ALPHA 0

typedef struct {
   unsigned char r;
   unsigned char g;
   unsigned char b;
} Col;

static const Col _colors[8][4] =
{
   { /* Red */
        { 0x44, 0x04, 0x00 },
        { 0x5c, 0x04, 0x00 },
        { 0x7c, 0x00, 0x00 },
        { 0xa4, 0x00, 0x00 },
   },
   { /* Blue */
        { 0x00, 0x04, 0x4c },
        { 0x00, 0x14, 0x6c },
        { 0x00, 0x24, 0x94 },
        { 0x00, 0x3c, 0xc0 }
   },
   { /* Green */
        { 0x00, 0x3c, 0xc0 },
        { 0x04, 0x54, 0x2c },
        { 0x14, 0x84, 0x5c },
        { 0x2c, 0xb4, 0x94 }
   },
   { /* Violet */
        { 0x2c, 0x08, 0x2c },
        { 0x50, 0x10, 0x4c },
        { 0x74, 0x30, 0x84 },
        { 0x98, 0x48, 0xb0 }
   },
   { /* Orange */
        { 0x6e, 0x20, 0x0c },
        { 0x98, 0x38, 0x10 },
        { 0xc4, 0x58, 0x10 },
        { 0xf0, 0x84, 0x14 }
   },
   { /* Black */
        { 0x0c, 0x0c, 0x14 },
        { 0x14, 0x14, 0x20 },
        { 0x1c, 0x1c, 0x2c },
        { 0x28, 0x28, 0x3c }
   },
   { /* White */
        { 0x24, 0x28, 0x4c },
        { 0x54, 0x54, 0x80 },
        { 0x98, 0x98, 0xb4 },
        { 0xe0, 0xe0, 0xe0 }
   },
   { /* Yellow */
        { 0xb4, 0x74, 0x00 },
        { 0xcc, 0xa0, 0x10 },
        { 0xe4, 0xcc, 0x28 },
        { 0xfc, 0xfc, 0x48 }
   }
};


static void
_sprites_colorize(Pud_Color    *sprite,
                  unsigned int  size,
                  Pud_Player    color)
{
   unsigned int i, k;
   Pud_Color *col;

   if (color == PUD_PLAYER_RED) return;

   for (k = 0; k < size; ++k)
     {
        col = &(sprite[k]);
        for (i = 0; i < 4; ++i)
          {
             if (!memcmp(col, &(_colors[0][i]), sizeof(Col)))
               {
                  memcpy(col, &(_colors[color][i]), sizeof(Col));
                  break;
               }
          }
     }
}


static Pud_Bool
_sprites_entries_parse(War2_Data                *w2,
                       War2_Sprites_Descriptor  *ud,
                       const unsigned int       *entries,
                       War2_Sprites_Decode_Func  func)
{
   unsigned char *ptr;
   uint16_t count, i, oline, max_w, max_h;
   uint8_t x, y, w, h, c;
   uint32_t dstart;
   size_t size, max_size;
   unsigned int offset, l, pcount, k;
   unsigned char *img = NULL, *pimg, *rows, *o;
   Pud_Color *img_rgba = NULL;

   /* If no callback has been specified, do nothing */
   if (!func)
     {
        WAR2_VERBOSE(w2, 1, "Warning: No callback specified.");
        return PUD_TRUE;
     }

   /* Palette */
   ptr = war2_palette_extract(w2, entries[0]);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to get palette");
   war2_palette_convert(ptr, ud->palette);
   free(ptr);

   ptr = war2_entry_extract(w2, entries[1], &size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to extract entry");

   memcpy(&count, &(ptr[0]), sizeof(uint16_t));
   memcpy(&max_w, &(ptr[2]), sizeof(uint16_t));
   memcpy(&max_h, &(ptr[4]), sizeof(uint16_t));

   max_size = max_w * max_h;
   img = malloc(max_size * sizeof(unsigned char));
   img_rgba = malloc(max_size * sizeof(Pud_Color));

   for (i = 0, offset = 6; i < count; ++i, offset += 8)
     {
        memcpy(&x, &(ptr[offset + 0]), sizeof(uint8_t));
        memcpy(&y, &(ptr[offset + 1]), sizeof(uint8_t));
        memcpy(&w, &(ptr[offset + 2]), sizeof(uint8_t));
        memcpy(&h, &(ptr[offset + 3]), sizeof(uint8_t));
        memcpy(&dstart, &(ptr[offset + 4]), sizeof(uint32_t));

        rows = ptr + dstart;
        pimg = img;

        for (l = 0; l < h; ++l)
          {
             memcpy(&oline, rows + (l * sizeof(uint16_t)), sizeof(uint16_t));
             o = rows + oline;

             for (pcount = 0; pcount < w;)
               {
                  c = *(o++);
                  if (c & RLE_REPEAT)
                    {
                       /* Repeat the next byte (c \ RLE_REPEAT) times as pixel value */
                       c &= 0x3f;
                       memset(&(pimg[pcount]), *(o++), c);
                       pcount += c;
                    }
                  else if (c & RLE_LEAVE)
                    {
                       /* Leave (c \ RLE_LEAVE) pixels transparent */
                       c &= 0x7f;
                       memset(&(pimg[pcount]), PALETTE_ALPHA, c);
                       pcount += c;
                    }
                  else
                    {
                       /* Take the next (c) bytes as pixel values */
                       memcpy(&(pimg[pcount]), o, c);
                       pcount += c;
                       o += c;
                    }
               }
             pimg += pcount;
          }

        size = w * h;
        for (k = 0; k < size; ++k)
          img_rgba[k] = ud->palette[img[k]];

        _sprites_colorize(img_rgba, size, ud->color);
        func(img_rgba, w, h, ud, i);
     }

   free(img_rgba);
   free(img);
   free(ptr);

   return PUD_TRUE;
}

War2_Sprites_Descriptor *
war2_sprites_decode_entry(War2_Data *w2,
                          Pud_Player                player_color,
                          Pud_Era                   era,
                          unsigned int              entry,
                          War2_Sprites_Decode_Func  func)
{
   War2_Sprites_Descriptor *ud;
   unsigned int entries[2] = { 0, entry };

   switch (era)
     {
      case PUD_ERA_FOREST:    entries[0] = 2;   break;
      case PUD_ERA_WINTER:    entries[0] = 18;  break;
      case PUD_ERA_WASTELAND: entries[0] = 10;  break;
      case PUD_ERA_SWAMP:     entries[0] = 438; break;
     }

   ud = calloc(1, sizeof(*ud));
   if (!ud) DIE_RETURN(NULL, "Failed to allocate memory");
   ud->era = era;
   ud->color = player_color;
   ud->object = entry;
   ud->sprite_type = 0;
   ud->side = 0;

   _sprites_entries_parse(w2, ud, entries, func);

   return ud;
}

War2_Sprites_Descriptor *
war2_sprites_decode(War2_Data                *w2,
                    Pud_Player                player_color,
                    Pud_Era                   era,
                    unsigned int              object,
                    War2_Sprites_Decode_Func  func)
{
   War2_Sprites_Descriptor *ud;
   unsigned int entries[2] = { 0, 0 };
   War2_Sprites type;
   Pud_Side side;

   if (object == WAR2_SPRITES_ICONS)
     type = WAR2_SPRITES_ICONS;
   else
     {
        switch (object)
          {
#define ORC_UNIT(val_) entries[1] = val_; type = WAR2_SPRITES_UNITS;  side = PUD_SIDE_ORC
#define HUMAN_UNIT(val_) entries[1] = val_; type = WAR2_SPRITES_UNITS; side = PUD_SIDE_HUMAN
#define NEUTRAL_UNIT(val_) entries[1] = val_; type = WAR2_SPRITES_UNITS; side = PUD_SIDE_NEUTRAL
#define ORC_START(val_) entries[1] = val_; type = WAR2_SPRITES_SYSTEM; side = PUD_SIDE_ORC
#define HUMAN_START(val_) entries[1] = val_; type = WAR2_SPRITES_SYSTEM; side = PUD_SIDE_HUMAN

#define ORC_UNIT_SWITCH(a_, b_, c_, d_) \
               { \
                  switch (era) { \
                   case PUD_ERA_FOREST:    entries[1] = a_; break; \
                   case PUD_ERA_WINTER:    entries[1] = b_; break; \
                   case PUD_ERA_WASTELAND: entries[1] = c_; break; \
                   case PUD_ERA_SWAMP:     entries[1] = d_; break; \
                  } \
                  type = WAR2_SPRITES_UNITS; side = PUD_SIDE_ORC; \
               }

#define HUMAN_UNIT_SWITCH(a_, b_, c_, d_) \
               { \
                  switch (era) { \
                   case PUD_ERA_FOREST:    entries[1] = a_; break; \
                   case PUD_ERA_WINTER:    entries[1] = b_; break; \
                   case PUD_ERA_WASTELAND: entries[1] = c_; break; \
                   case PUD_ERA_SWAMP:     entries[1] = d_; break; \
                  } \
                  type = WAR2_SPRITES_UNITS; side = PUD_SIDE_HUMAN; \
               }


#define ORC_BUILDING_SWITCH(a_, b_, c_, d_) \
               { \
                  switch (era) { \
                   case PUD_ERA_FOREST:    entries[1] = a_; break; \
                   case PUD_ERA_WINTER:    entries[1] = b_; break; \
                   case PUD_ERA_WASTELAND: entries[1] = c_; break; \
                   case PUD_ERA_SWAMP:     entries[1] = d_; break; \
                  } \
                  type = WAR2_SPRITES_BUILDINGS; side = PUD_SIDE_ORC; \
               }
#define HUMAN_BUILDING_SWITCH(a_, b_, c_, d_) \
               { \
                  switch (era) { \
                   case PUD_ERA_FOREST:    entries[1] = a_; break; \
                   case PUD_ERA_WINTER:    entries[1] = b_; break; \
                   case PUD_ERA_WASTELAND: entries[1] = c_; break; \
                   case PUD_ERA_SWAMP:     entries[1] = d_; break; \
                  } \
                  type = WAR2_SPRITES_BUILDINGS; side = PUD_SIDE_HUMAN; \
               }
#define NEUTRAL_BUILDING_SWITCH(a_, b_, c_, d_) \
               { \
                  switch (era) { \
                   case PUD_ERA_FOREST:    entries[1] = a_; break; \
                   case PUD_ERA_WINTER:    entries[1] = b_; break; \
                   case PUD_ERA_WASTELAND: entries[1] = c_; break; \
                   case PUD_ERA_SWAMP:     entries[1] = d_; break; \
                  } \
                  type = WAR2_SPRITES_BUILDINGS; side = PUD_SIDE_NEUTRAL; \
               }

             /* Units */
           case PUD_UNIT_DWARVES               : HUMAN_UNIT(33); break;
           case PUD_UNIT_GOBLIN_SAPPER         : ORC_UNIT(34); break;
           case PUD_UNIT_GRYPHON_RIDER         : HUMAN_UNIT(35); break;
           case PUD_UNIT_DRAGON                : ORC_UNIT(36); break;
           case PUD_UNIT_EYE_OF_KILROGG        : ORC_UNIT(37); break;
           case PUD_UNIT_GNOMISH_FLYING_MACHINE: HUMAN_UNIT(38); break;
           case PUD_UNIT_HUMAN_TRANSPORT       : HUMAN_UNIT(39); break;
           case PUD_UNIT_ORC_TRANSPORT         : ORC_UNIT(40); break;
           case PUD_UNIT_BATTLESHIP            : HUMAN_UNIT(41); break;
           case PUD_UNIT_JUGGERNAUGHT          : ORC_UNIT(42); break;
           case PUD_UNIT_GNOMISH_SUBMARINE     : HUMAN_UNIT_SWITCH(43, 43, 182, 526); break;
           case PUD_UNIT_GIANT_TURTLE          : ORC_UNIT_SWITCH(44, 44, 183, 527); break;
           case PUD_UNIT_INFANTRY              : HUMAN_UNIT(45); break;
           case PUD_UNIT_GRUNT                 : ORC_UNIT(46); break;
           case PUD_UNIT_PEASANT               : HUMAN_UNIT(47); break;
           case PUD_UNIT_PEON                  : ORC_UNIT(48); break;
           case PUD_UNIT_BALLISTA              : HUMAN_UNIT(49); break;
           case PUD_UNIT_CATAPULT              : ORC_UNIT(50); break;
           case PUD_UNIT_KNIGHT                : HUMAN_UNIT(51); break;
           case PUD_UNIT_OGRE                  : ORC_UNIT(52); break;
           case PUD_UNIT_ARCHER                : HUMAN_UNIT(53); break;
           case PUD_UNIT_AXETHROWER            : ORC_UNIT(54); break;
           case PUD_UNIT_MAGE                  : HUMAN_UNIT(55); break;
           case PUD_UNIT_DEATH_KNIGHT          : ORC_UNIT(58); break;
           case PUD_UNIT_HUMAN_TANKER          : HUMAN_UNIT(59); break;
           case PUD_UNIT_ORC_TANKER            : ORC_UNIT(60); break;
           case PUD_UNIT_ELVEN_DESTROYER       : HUMAN_UNIT(61); break;
           case PUD_UNIT_TROLL_DESTROYER       : ORC_UNIT(62); break;
           case PUD_UNIT_GOBLIN_ZEPPLIN        : HUMAN_UNIT(63); break;
           case PUD_UNIT_CRITTER_SHEEP         : NEUTRAL_UNIT(64); break;
           case PUD_UNIT_CRITTER_PIG           : NEUTRAL_UNIT(65); break;
           case PUD_UNIT_CRITTER_SEAL          : NEUTRAL_UNIT(66); break;
           case PUD_UNIT_SKELETON              : NEUTRAL_UNIT(69); break;
           case PUD_UNIT_DAEMON                : NEUTRAL_UNIT(70); break;

                                                 /* Start Locations */
           case PUD_UNIT_HUMAN_START           : HUMAN_START(164); break;
           case PUD_UNIT_ORC_START             : ORC_START(165); break;

                                                 /* Buildings */
           case PUD_UNIT_HUMAN_GUARD_TOWER   :   HUMAN_BUILDING_SWITCH( 80, 169,  80, 507); break;
           case PUD_UNIT_ORC_GUARD_TOWER     :     ORC_BUILDING_SWITCH( 81, 170,  81, 508); break;
           case PUD_UNIT_HUMAN_CANNON_TOWER  :   HUMAN_BUILDING_SWITCH( 82, 171,  82, 509); break;
           case PUD_UNIT_ORC_CANNON_TOWER    :     ORC_BUILDING_SWITCH( 83, 172,  83, 510); break;
           case PUD_UNIT_MAGE_TOWER          :   HUMAN_BUILDING_SWITCH( 84, 160,  84, 505); break;
           case PUD_UNIT_TEMPLE_OF_THE_DAMNED:     ORC_BUILDING_SWITCH( 85, 161,  85, 506); break;
           case PUD_UNIT_KEEP                :   HUMAN_BUILDING_SWITCH( 86, 128,  86, 473); break;
           case PUD_UNIT_STRONGHOLD          :     ORC_BUILDING_SWITCH( 87, 129,  87, 474); break;
           case PUD_UNIT_GRYPHON_AVIARY      :   HUMAN_BUILDING_SWITCH( 88, 130,  88, 475); break;
           case PUD_UNIT_DRAGON_ROOST        :     ORC_BUILDING_SWITCH( 89, 131,  89, 476); break;
           case PUD_UNIT_GNOMISH_INVENTOR    :   HUMAN_BUILDING_SWITCH( 90, 132,  90, 477); break;
           case PUD_UNIT_GOBLIN_ALCHEMIST    :     ORC_BUILDING_SWITCH( 91, 133,  91, 478); break;
           case PUD_UNIT_FARM                :   HUMAN_BUILDING_SWITCH( 92, 134, 173, 479); break;
           case PUD_UNIT_PIG_FARM            :     ORC_BUILDING_SWITCH( 93, 135, 174, 480); break;
           case PUD_UNIT_HUMAN_BARRACKS      :   HUMAN_BUILDING_SWITCH( 94, 136,  94, 481); break;
           case PUD_UNIT_ORC_BARRACKS        :     ORC_BUILDING_SWITCH( 95, 137,  95, 482); break;
           case PUD_UNIT_CHURCH              :   HUMAN_BUILDING_SWITCH( 96, 138,  96, 483); break;
           case PUD_UNIT_ALTAR_OF_STORMS     :     ORC_BUILDING_SWITCH( 97, 139,  97, 484); break;
           case PUD_UNIT_HUMAN_SCOUT_TOWER   :   HUMAN_BUILDING_SWITCH( 98, 140,  98, 485); break;
           case PUD_UNIT_ORC_SCOUT_TOWER     :     ORC_BUILDING_SWITCH( 99, 141,  99, 486); break;
           case PUD_UNIT_TOWN_HALL           :   HUMAN_BUILDING_SWITCH(100, 142, 100, 487); break;
           case PUD_UNIT_GREAT_HALL          :     ORC_BUILDING_SWITCH(101, 143, 101, 488); break;
           case PUD_UNIT_ELVEN_LUMBER_MILL   :   HUMAN_BUILDING_SWITCH(102, 144, 175, 489); break;
           case PUD_UNIT_TROLL_LUMBER_MILL   :     ORC_BUILDING_SWITCH(103, 145, 176, 490); break;
           case PUD_UNIT_STABLES             :   HUMAN_BUILDING_SWITCH(104, 146, 104, 491); break;
           case PUD_UNIT_OGRE_MOUND          :     ORC_BUILDING_SWITCH(105, 147, 105, 492); break;
           case PUD_UNIT_HUMAN_BLACKSMITH    :   HUMAN_BUILDING_SWITCH(106, 148, 106, 493); break;
           case PUD_UNIT_ORC_BLACKSMITH      :     ORC_BUILDING_SWITCH(107, 149, 107, 494); break;
           case PUD_UNIT_HUMAN_SHIPYARD      :   HUMAN_BUILDING_SWITCH(108, 150, 108, 495); break;
           case PUD_UNIT_ORC_SHIPYARD        :     ORC_BUILDING_SWITCH(109, 151, 109, 496); break;
           case PUD_UNIT_HUMAN_FOUNDRY       :   HUMAN_BUILDING_SWITCH(110, 152, 110, 497); break;
           case PUD_UNIT_ORC_FOUNDRY         :     ORC_BUILDING_SWITCH(111, 153, 111, 498); break;
           case PUD_UNIT_HUMAN_REFINERY      :   HUMAN_BUILDING_SWITCH(112, 154, 112, 499); break;
           case PUD_UNIT_ORC_REFINERY        :     ORC_BUILDING_SWITCH(113, 155, 113, 500); break;
           case PUD_UNIT_HUMAN_OIL_WELL      :   HUMAN_BUILDING_SWITCH(114, 156, 177, 501); break;
           case PUD_UNIT_ORC_OIL_WELL        :     ORC_BUILDING_SWITCH(115, 157, 178, 502); break;
           case PUD_UNIT_CASTLE              :   HUMAN_BUILDING_SWITCH(116, 158, 116, 503); break;
           case PUD_UNIT_FORTRESS            :     ORC_BUILDING_SWITCH(117, 159, 117, 504); break;
           case PUD_UNIT_OIL_PATCH           : NEUTRAL_BUILDING_SWITCH(118, 118, 180, 515); break;
           case PUD_UNIT_GOLD_MINE           : NEUTRAL_BUILDING_SWITCH(119, 162, 179, 511); break;
           case PUD_UNIT_DARK_PORTAL         : NEUTRAL_BUILDING_SWITCH(167, 184, 185, 513); break;
           case PUD_UNIT_RUNESTONE           : NEUTRAL_BUILDING_SWITCH(181, 186, 181, 514); break;
           case PUD_UNIT_CIRCLE_OF_POWER     : NEUTRAL_BUILDING_SWITCH(166, 166, 166, 525); break;
          }
     }

   /* Set entry for palette conversion */
   switch (era)
     {
      case PUD_ERA_FOREST:    entries[0] = 2;   if (type == WAR2_SPRITES_ICONS) entries[1] = 356; break;
      case PUD_ERA_WINTER:    entries[0] = 18;  if (type == WAR2_SPRITES_ICONS) entries[1] = 357; break;
      case PUD_ERA_WASTELAND: entries[0] = 10;  if (type == WAR2_SPRITES_ICONS) entries[1] = 358; break;
      case PUD_ERA_SWAMP:     entries[0] = 438; if (type == WAR2_SPRITES_ICONS) entries[1] = 471; break;
     }

   /* Check object is valid (assigned entriy) */
   if (entries[1] == 0)
     DIE_RETURN(NULL, "Invalid object [%u]", object);
   WAR2_VERBOSE(w2, 1, "Decoding entry [%i] for object [%u]", entries[1], object);

   /* Alloc */
   ud = calloc(1, sizeof(*ud));
   if (!ud) DIE_RETURN(NULL, "Failed to allocate memory");
   ud->era = era;
   ud->color = player_color;
   ud->object = object;
   ud->sprite_type = type;
   ud->side = side;

   _sprites_entries_parse(w2, ud, entries, func);

   return ud;
}

void
war2_sprites_descriptor_free(War2_Sprites_Descriptor *ud)
{
   free(ud);
}


