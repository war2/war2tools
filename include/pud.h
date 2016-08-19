/*
 * pud.h
 * libpud
 *
 * Copyright (c) 2014 -2016 Jean Guyomarc'h
 */

#ifndef _PUD_H_
#define _PUD_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char Pud_Bool;
#define PUD_TRUE  ((Pud_Bool)(1))
#define PUD_FALSE ((Pud_Bool)(0))

#define PUD_VERSION_TOD  0x0011
#define PUD_VERSION_BDP  0x0013

typedef enum
{
   PUD_SECTION_TYPE     = 0,
   PUD_SECTION_VER      = 1,
   PUD_SECTION_DESC     = 2,
   PUD_SECTION_OWNR     = 3,
   PUD_SECTION_ERA      = 4,
   PUD_SECTION_ERAX     = 5,
   PUD_SECTION_DIM      = 6,
   PUD_SECTION_UDTA     = 7,
   PUD_SECTION_ALOW     = 8,
   PUD_SECTION_UGRD     = 9,
   PUD_SECTION_SIDE     = 10,
   PUD_SECTION_SGLD     = 11,
   PUD_SECTION_SLBR     = 12,
   PUD_SECTION_SOIL     = 13,
   PUD_SECTION_AIPL     = 14,
   PUD_SECTION_MTXM     = 15,
   PUD_SECTION_SQM      = 16,
   PUD_SECTION_OILM     = 17,
   PUD_SECTION_REGM     = 18,
   PUD_SECTION_UNIT     = 19
} Pud_Section;

typedef enum
{
   PUD_DIMENSIONS_UNDEFINED = 0,
   PUD_DIMENSIONS_32_32,
   PUD_DIMENSIONS_64_64,
   PUD_DIMENSIONS_96_96,
   PUD_DIMENSIONS_128_128
} Pud_Dimensions;

typedef enum
{
   PUD_PLAYER_RED       = 0,
   PUD_PLAYER_BLUE      = 1,
   PUD_PLAYER_GREEN     = 2,
   PUD_PLAYER_VIOLET    = 3,
   PUD_PLAYER_ORANGE    = 4,
   PUD_PLAYER_BLACK     = 5,
   PUD_PLAYER_WHITE     = 6,
   PUD_PLAYER_YELLOW    = 7,

   PUD_PLAYER_NEUTRAL   = 15
} Pud_Player;

typedef enum
{
   PUD_UPGRADE_SWORD_1          = 0,
   PUD_UPGRADE_SWORD_2,
   PUD_UPGRADE_AXE_1,
   PUD_UPGRADE_AXE_2,
   PUD_UPGRADE_ARROW_1,
   PUD_UPGRADE_ARROW_2,
   PUD_UPGRADE_SPEAR_1,
   PUD_UPGRADE_SPEAR_2,
   PUD_UPGRADE_HUMAN_SHIELD_1,
   PUD_UPGRADE_HUMAN_SHIELD_2,
   PUD_UPGRADE_ORC_SHIELD_1,
   PUD_UPGRADE_ORC_SHIELD_2,
   PUD_UPGRADE_HUMAN_SHIP_CANNON_1,
   PUD_UPGRADE_HUMAN_SHIP_CANNON_2,
   PUD_UPGRADE_ORC_SHIP_CANNON_1,
   PUD_UPGRADE_ORC_SHIP_CANNON_2,
   PUD_UPGRADE_HUMAN_SHIP_ARMOR_1,
   PUD_UPGRADE_HUMAN_SHIP_ARMOR_2,
   PUD_UPGRADE_ORC_SHIP_ARMOR_1,
   PUD_UPGRADE_ORC_SHIP_ARMOR_2,
   PUD_UPGRADE_CATAPULT_1,
   PUD_UPGRADE_CATAPULT_2,
   PUD_UPGRADE_BALLISTA_1,
   PUD_UPGRADE_BALLISTA_2,
   PUD_UPGRADE_TRAIN_RANGERS,
   PUD_UPGRADE_LONGBOW,
   PUD_UPGRADE_RANGER_SCOUTING,
   PUD_UPGRADE_RANGER_MARKSMANSHIP,
   PUD_UPGRADE_TRAIN_BERSERKERS,
   PUD_UPGRADE_LIGHTER_AXES,
   PUD_UPGRADE_BERSERKER_SCOUTING,
   PUD_UPGRADE_BERSERKER_REGENERATION,
   PUD_UPGRADE_TRAIN_OGRE_MAGES,
   PUD_UPGRADE_TRAIN_PALADINS,
   PUD_UPGRADE_HOLY_VISION,
   PUD_UPGRADE_HEALING,
   PUD_UPGRADE_EXORCISM,
   PUD_UPGRADE_FLAME_SHIELD,
   PUD_UPGRADE_FIREBALL,
   PUD_UPGRADE_SLOW,
   PUD_UPGRADE_INVISIBILITY,
   PUD_UPGRADE_POLYMORPH,
   PUD_UPGRADE_BLIZZARD,
   PUD_UPGRADE_EYE_OF_KILROGG,
   PUD_UPGRADE_BLOODLUST,
   PUD_UPGRADE_RAISE_DEAD,
   PUD_UPGRADE_DEATH_COIL,
   PUD_UPGRADE_WHIRLWIND,
   PUD_UPGRADE_HASTE,
   PUD_UPGRADE_UNHOLY_ARMOR,
   PUD_UPGRADE_RUNES,
   PUD_UPGRADE_DEATH_AND_DECAY
} Pud_Upgrade;

typedef enum
{
   PUD_UNIT_INFANTRY                    = 0x00,
   PUD_UNIT_GRUNT                       = 0x01,
   PUD_UNIT_PEASANT                     = 0x02,
   PUD_UNIT_PEON                        = 0x03,
   PUD_UNIT_BALLISTA                    = 0x04,
   PUD_UNIT_CATAPULT                    = 0x05,
   PUD_UNIT_KNIGHT                      = 0x06,
   PUD_UNIT_OGRE                        = 0x07,
   PUD_UNIT_ARCHER                      = 0x08,
   PUD_UNIT_AXETHROWER                  = 0x09,
   PUD_UNIT_MAGE                        = 0x0a,
   PUD_UNIT_DEATH_KNIGHT                = 0x0b,
   PUD_UNIT_PALADIN                     = 0x0c,
   PUD_UNIT_OGRE_MAGE                   = 0x0d,
   PUD_UNIT_DWARVES                     = 0x0e,
   PUD_UNIT_GOBLIN_SAPPER               = 0x0f,
   PUD_UNIT_ATTACK_PEASANT              = 0x10,
   PUD_UNIT_ATTACK_PEON                 = 0x11,
   PUD_UNIT_RANGER                      = 0x12,
   PUD_UNIT_BERSERKER                   = 0x13,
   PUD_UNIT_ALLERIA                     = 0x14,
   PUD_UNIT_TERON_GOREFIEND             = 0x15,
   PUD_UNIT_KURDRAN_AND_SKY_REE         = 0x16,
   PUD_UNIT_DENTARG                     = 0x17,
   PUD_UNIT_KHADGAR                     = 0x18,
   PUD_UNIT_GROM_HELLSCREAM             = 0x19,
   PUD_UNIT_HUMAN_TANKER                = 0x1a,
   PUD_UNIT_ORC_TANKER                  = 0x1b,
   PUD_UNIT_HUMAN_TRANSPORT             = 0x1c,
   PUD_UNIT_ORC_TRANSPORT               = 0x1d,
   PUD_UNIT_ELVEN_DESTROYER             = 0x1e,
   PUD_UNIT_TROLL_DESTROYER             = 0x1f,
   PUD_UNIT_BATTLESHIP                  = 0x20,
   PUD_UNIT_JUGGERNAUGHT                = 0x21,
   PUD_UNIT_DEATHWING                   = 0x23,
   PUD_UNIT_GNOMISH_SUBMARINE           = 0x26,
   PUD_UNIT_GIANT_TURTLE                = 0x27,
   PUD_UNIT_GNOMISH_FLYING_MACHINE      = 0x28,
   PUD_UNIT_GOBLIN_ZEPPLIN              = 0x29,
   PUD_UNIT_GRYPHON_RIDER               = 0x2a,
   PUD_UNIT_DRAGON                      = 0x2b,
   PUD_UNIT_TURALYON                    = 0x2c,
   PUD_UNIT_EYE_OF_KILROGG              = 0x2d,
   PUD_UNIT_DANATH                      = 0x2e,
   PUD_UNIT_KARGATH_BLADEFIST           = 0x2f,
   PUD_UNIT_CHO_GALL                    = 0x31,
   PUD_UNIT_LOTHAR                      = 0x32,
   PUD_UNIT_GUL_DAN                     = 0x33,
   PUD_UNIT_UTHER_LIGHTBRINGER          = 0x34,
   PUD_UNIT_ZUL_JIN                     = 0x35,
   PUD_UNIT_SKELETON                    = 0x37,
   PUD_UNIT_DAEMON                      = 0x38,
   PUD_UNIT_CRITTER                     = 0x39,
   PUD_UNIT_FARM                        = 0x3a,
   PUD_UNIT_PIG_FARM                    = 0x3b,
   PUD_UNIT_HUMAN_BARRACKS              = 0x3c,
   PUD_UNIT_ORC_BARRACKS                = 0x3d,
   PUD_UNIT_CHURCH                      = 0x3e,
   PUD_UNIT_ALTAR_OF_STORMS             = 0x3f,
   PUD_UNIT_HUMAN_SCOUT_TOWER           = 0x40,
   PUD_UNIT_ORC_SCOUT_TOWER             = 0x41,
   PUD_UNIT_STABLES                     = 0x42,
   PUD_UNIT_OGRE_MOUND                  = 0x43,
   PUD_UNIT_GNOMISH_INVENTOR            = 0x44,
   PUD_UNIT_GOBLIN_ALCHEMIST            = 0x45,
   PUD_UNIT_GRYPHON_AVIARY              = 0x46,
   PUD_UNIT_DRAGON_ROOST                = 0x47,
   PUD_UNIT_HUMAN_SHIPYARD              = 0x48,
   PUD_UNIT_ORC_SHIPYARD                = 0x49,
   PUD_UNIT_TOWN_HALL                   = 0x4a,
   PUD_UNIT_GREAT_HALL                  = 0x4b,
   PUD_UNIT_ELVEN_LUMBER_MILL           = 0x4c,
   PUD_UNIT_TROLL_LUMBER_MILL           = 0x4d,
   PUD_UNIT_HUMAN_FOUNDRY               = 0x4e,
   PUD_UNIT_ORC_FOUNDRY                 = 0x4f,
   PUD_UNIT_MAGE_TOWER                  = 0x50,
   PUD_UNIT_TEMPLE_OF_THE_DAMNED        = 0x51,
   PUD_UNIT_HUMAN_BLACKSMITH            = 0x52,
   PUD_UNIT_ORC_BLACKSMITH              = 0x53,
   PUD_UNIT_HUMAN_REFINERY              = 0x54,
   PUD_UNIT_ORC_REFINERY                = 0x55,
   PUD_UNIT_HUMAN_OIL_WELL              = 0x56,
   PUD_UNIT_ORC_OIL_WELL                = 0x57,
   PUD_UNIT_KEEP                        = 0x58,
   PUD_UNIT_STRONGHOLD                  = 0x59,
   PUD_UNIT_CASTLE                      = 0x5a,
   PUD_UNIT_FORTRESS                    = 0x5b,
   PUD_UNIT_GOLD_MINE                   = 0x5c,
   PUD_UNIT_OIL_PATCH                   = 0x5d,
   PUD_UNIT_HUMAN_START                 = 0x5e,
   PUD_UNIT_ORC_START                   = 0x5f,
   PUD_UNIT_HUMAN_GUARD_TOWER           = 0x60,
   PUD_UNIT_ORC_GUARD_TOWER             = 0x61,
   PUD_UNIT_HUMAN_CANNON_TOWER          = 0x62,
   PUD_UNIT_ORC_CANNON_TOWER            = 0x63,
   PUD_UNIT_CIRCLE_OF_POWER             = 0x64,
   PUD_UNIT_DARK_PORTAL                 = 0x65,
   PUD_UNIT_RUNESTONE                   = 0x66,
   PUD_UNIT_HUMAN_WALL                  = 0x67,
   PUD_UNIT_ORC_WALL                    = 0x68,

   /* Custom types */
   PUD_UNIT_CRITTER_SHEEP               = 0x69,
   PUD_UNIT_CRITTER_PIG                 = 0x6a,
   PUD_UNIT_CRITTER_SEAL                = 0x6b,
   PUD_UNIT_CRITTER_RED_PIG             = 0x6c,

   /* Sentinel. Do not touch 0x7f. war2edit heavily relies on this value */
   PUD_UNIT_NONE                        = 0x7f
} Pud_Unit;

typedef enum
{
   PUD_OWNER_NOBODY                     = 0x03,
   PUD_OWNER_PASSIVE_COMPUTER           = 0x02,
   PUD_OWNER_COMPUTER                   = 0x04,
   PUD_OWNER_HUMAN                      = 0x05,
   PUD_OWNER_RESCUE_PASSIVE             = 0x06,
   PUD_OWNER_RESCUE_ACTIVE              = 0x07
} Pud_Owner;

typedef enum
{
   PUD_SIDE_HUMAN                       = 0x00,
   PUD_SIDE_ORC                         = 0x01,
   PUD_SIDE_NEUTRAL                     = 0x02
} Pud_Side;

typedef enum
{
   PUD_OPEN_MODE_R = (1 << 0),
   PUD_OPEN_MODE_W = (1 << 1),
} Pud_Open_Mode;

typedef enum
{
   PUD_ERA_FOREST       = 0,
   PUD_ERA_WINTER       = 1,
   PUD_ERA_WASTELAND    = 2,
   PUD_ERA_SWAMP        = 3
} Pud_Era;

typedef enum
{
   PUD_AI_LAND_ATTACK    = 0x00,
   PUD_AI_PASSIVE        = 0x01,
   PUD_AI_SEA_ATTACK     = 0x19,
   PUD_AI_AIR_ATTACK     = 0x1a,

   PUD_AI_ORC_3          = 0x02,
   PUD_AI_ORC_4          = 0x04,
   PUD_AI_ORC_5          = 0x06,
   PUD_AI_ORC_6          = 0x08,
   PUD_AI_ORC_7          = 0x0a,
   PUD_AI_ORC_8          = 0x0c,
   PUD_AI_ORC_9          = 0x0e,
   PUD_AI_ORC_10         = 0x10,
   PUD_AI_ORC_11         = 0x12,
   PUD_AI_ORC_12         = 0x14,
   PUD_AI_ORC_13         = 0x16,
   PUD_AI_HUMAN_4        = 0x03,
   PUD_AI_HUMAN_5        = 0x05,
   PUD_AI_HUMAN_6        = 0x07,
   PUD_AI_HUMAN_7        = 0x09,
   PUD_AI_HUMAN_8        = 0x0b,
   PUD_AI_HUMAN_9        = 0x0d,
   PUD_AI_HUMAN_10       = 0x0f,
   PUD_AI_HUMAN_11       = 0x11,
   PUD_AI_HUMAN_12       = 0x13,
   PUD_AI_HUMAN_13       = 0x15,

   PUD_AI_HUMAN_14_RED   = 0x1b,
   PUD_AI_HUMAN_14_WHITE = 0x1c,
   PUD_AI_HUMAN_14_BLACK = 0x1d,
   PUD_AI_ORC_14_GREEN   = 0x1e,
   PUD_AI_ORC_14_WHITE   = 0x1f,

   PUD_AI_EXPANSION_1    = 0x20,
   PUD_AI_EXPANSION_51   = 0x52
} Pud_AI;

typedef struct _Pud Pud;

struct _Pud
{
   Pud_Open_Mode  open_mode;
   unsigned char *mem_map;
   unsigned char *ptr;
   size_t         mem_map_size;
   char          *filename;

   uint32_t       tag;
   uint16_t       version;
   char           description[32];
   uint8_t        era;
   Pud_Dimensions dims;

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } owner; /* [defaults] */

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } side; /* [defaults] */

   struct {
      uint16_t players[8];
      uint16_t unusable[7];
      uint16_t neutral;
   } sgld, slbr, soil; /* [defaults] */

   struct {
      uint8_t players[8];
      uint8_t unusable[7];
      uint8_t neutral;
   } ai; /* [defaults] */

   struct _alow {
      uint32_t players[8];
      uint32_t unusable[7];
      uint32_t neutral;
   } unit_alow, spell_start, spell_alow, spell_acq, up_alow, up_acq; /* [defaults] */

   struct _ugrd {
      uint8_t           time;
      uint16_t          gold;
      uint16_t          lumber;
      uint16_t          oil;
      uint16_t          icon;
      uint16_t          group;
      uint32_t          flags;
   } upgrade[52]; /* [defaults] */

   /* Cache values */
   unsigned int map_w;
   unsigned int map_h;
   unsigned int tiles; /* pud->map_w * pud->map_h */

   /* Number of elements is in 'tiles' */
   uint16_t *tiles_map;
   uint16_t *action_map;
   uint16_t *movement_map;
   uint8_t *oil_map;

   struct _Pud_Unit {
      uint16_t x;
      uint16_t y;
      uint8_t  type;
      uint8_t  owner;
      uint16_t alter;
   } *units;

   unsigned int units_count;

   struct _unit_data {
      uint32_t     sight;
      uint16_t     hp;
      uint8_t      build_time;
      uint8_t      gold_cost;
      uint8_t      lumber_cost;
      uint8_t      oil_cost;
      uint8_t      range;
      uint8_t      computer_react_range;
      uint8_t      human_react_range;
      uint8_t      armor;
      uint8_t      basic_damage;
      uint8_t      piercing_damage;
      uint8_t      decay_rate;
      uint8_t      annoy;
      uint8_t      has_magic;
      uint8_t      weapons_upgradable;
      uint8_t      armor_upgradable;

      uint8_t      missile_weapon;
      uint8_t      type;
      uint8_t      mouse_right_btn;
      uint8_t      rect_sel;
      uint8_t      priority;
      uint16_t     point_value;
      uint8_t      can_target;
      uint32_t     flags;
      uint16_t     overlap_frames;
      uint16_t     size_w;
      uint16_t     size_h;
      uint16_t     box_w;
      uint16_t     box_h;
   } unit_data[110]; /* [defaults] */

   /* Bitfield: is section X present? */
   uint32_t     sections;

   /* Cache */
   uint8_t       current_section;

   Pud_Bool has_erax;

   unsigned int  verbose        : 3;
   unsigned int  init           : 1; /* set by defaults */
   unsigned int  default_allow  : 1; /* [defaults] */
   unsigned int  default_udta   : 1; /* [defaults] */
   unsigned int  default_ugrd   : 1; /* [defaults] */
   unsigned int  extension_pack : 1;
};

typedef struct _Pud_Color Pud_Color;

struct _Pud_Color
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
   unsigned char a;
};

typedef enum
{
   PUD_ERROR_NONE = 0,
   PUD_ERROR_INVALID_START_LOCATIONS,
   PUD_ERROR_NOT_INITIALIZED
} Pud_Error;

Pud_Bool pud_init(void);
void pud_shutdown(void);

Pud *pud_open_new(const char *file, Pud_Open_Mode mode);
Pud *pud_open(const char *file, Pud_Open_Mode mode);
void pud_close(Pud *pud);
Pud_Bool pud_reopen(Pud *pud, const char *file, Pud_Open_Mode mode);
Pud_Bool pud_parse(Pud *pud);
void pud_verbose_set(Pud *pud, int lvl);
Pud_Bool pud_section_is_optional(Pud_Section sec);
uint32_t pud_go_to_section(Pud *pud, Pud_Section sec);
void pud_print(Pud *pud, FILE *stream);
void pud_dimensions_to_size(Pud_Dimensions dim, unsigned int *x_ret, unsigned int *y_ret);
Pud_Owner pud_owner_convert(uint8_t code);
Pud_Side pud_side_convert(uint8_t code);
void pud_version_set(Pud *pud, uint16_t version);
void pud_description_set(Pud *pud, char descr[32]);
void pud_tag_set(Pud *pud, uint32_t tag);
Pud_Error pud_check(Pud *pud);
Pud_Bool pud_defaults_set(Pud *pud);
Pud_Bool pud_write(const Pud *pud, const char *file);
int pud_unit_add(Pud *pud, uint16_t x, uint16_t y, Pud_Player owner, Pud_Unit type, uint16_t alter);
void pud_era_set(Pud *pud, Pud_Era era);
void pud_dimensions_set(Pud *pud, Pud_Dimensions dims);
void pud_tag_generate(Pud *pud);

void *pud_mmap(const char *file, size_t *size_ret);
void pud_munmap(void *map, size_t size);

unsigned char *pud_minimap_bitmap_generate(Pud *pud, int *size_ret);

Pud_Bool pud_minimap_to_ppm(Pud *pud, const char *file);

const char *pud_section_at_index(int idx);

const char *pud_unit2str(Pud_Unit unit, Pud_Bool pretty);
const char *pud_era2str(Pud_Era era);
const char *pud_color2str(Pud_Player color);

Pud_Bool pud_section_exists(char sec[4]);
uint16_t pud_tile_at(Pud *pud, int x, int y);

Pud_Bool pud_tile_set(Pud *pud, uint16_t x, uint16_t y, uint16_t tile);
uint16_t pud_tile_get(const Pud *pud, unsigned int x, unsigned int y);
Pud_Bool pud_unit_building_is(Pud_Unit unit);

Pud_Bool pud_unit_start_location_is(Pud_Unit unit);
Pud_Bool pud_unit_flying_is(Pud_Unit unit);
Pud_Bool pud_unit_underwater_is(Pud_Unit unit);
Pud_Bool pud_unit_land_is(Pud_Unit unit);
Pud_Bool pud_unit_marine_is(Pud_Unit unit);
Pud_Bool pud_unit_always_passive_is(Pud_Unit unit);
Pud_Bool pud_unit_oil_well_is(Pud_Unit unit);
Pud_Bool pud_unit_coast_building_is(Pud_Unit unit);

Pud_Color pud_tile_to_color(Pud_Era era, uint16_t tile);
Pud_Color pud_color_for_player(Pud_Player player);
Pud_Color pud_gold_mine_color_get(void);
Pud_Color pud_oil_patch_color_get(void);
Pud_Color pud_color_for_unit(Pud_Unit unit, Pud_Player player);

Pud_Side pud_unit_side_get(Pud_Unit unit);

uint8_t pud_random_get(const uint16_t tile);

#ifdef __cplusplus
}
#endif
#endif /* ! _PUD_H_ */
